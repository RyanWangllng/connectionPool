#include "connectionPool.h"

// 线程安全的懒汉单例接口
ConnectionPool* ConnectionPool::getConnectionPool() {
    // 由编译器进行lock和unlock
    static ConnectionPool pool;
    return &pool;
}

// 从配置文件中加载配置项
bool ConnectionPool::loadConfigFile() {
    FILE *pf = fopen("/root/connectionPool/connectionPool/src/mysql.cnf", "r");

    if (pf == nullptr) {
        LOG("mysql.cnf file is not exist!");
        return false;
    }

    while (!feof(pf)) {
        char line[1024] = {0};
        fgets(line, 1024, pf);
        std::string str = line;

        int idx = str.find('=', 0);
        if (idx == -1) { // 不是配置项
            continue;
        }
        int endidx = str.find('\n', idx);
        std::string key = str.substr(0, idx);
        std::string value = str.substr(idx + 1, endidx - idx - 1);

        if (key == "ip") {
            _ip = value;
        } else if (key == "port") {
            _port = atoi(value.c_str());
        } else if (key == "username") {
            _username = value;
        } else if (key == "password") {
            _password = value;
        } else if (key == "dbname") {
            _dbname = value;
        } else if (key == "initSize") {
            _initSzie = atoi(value.c_str());
        } else if (key == "maxSize") {
            _maxSize = atoi(value.c_str());
        } else if (key == "maxIdleTime") {
            _maxIdleTime = atoi(value.c_str());
        } else if (key == "connectionTimeout") {
            _connectionTimeout = atoi(value.c_str());
        }
    }
    return true;
}

// 第一次获取单例实例时，调用一下构造函数
ConnectionPool::ConnectionPool() {
    // 加载配置项
    if (!loadConfigFile()) return;

    // 创建初始数量的连接
    for (int i = 0; i < _initSzie; ++i) {
        Connection *p = new Connection;
        p->connect(_ip, _port, _username, _password, _dbname);
        p->updateAliveTime(); // 更新开始空闲的起始时间
        _connectionQue.push(p);
        ++_connectionCnt;
    }

    // 启动一个新的线程，作为连接的生产者
    std::thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    // 启动一个新的定时线程，扫描多余的空闲连接，超过maxIdleTime的连接进行回收
    std::thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

// 运行在独立的线程中，专门生产新连接
void ConnectionPool::produceConnectionTask() {
    for (;;) {
        std::unique_lock<std::mutex> ulock(_queueMutex);
        while (!_connectionQue.empty()) {
            cv.wait(ulock); // 队列不空，生产线程等待，不用生产
        }

        // 连接数量没有到达上线，继续创建
        if (_connectionCnt < _maxSize) {
            Connection *p = new Connection;
            p->connect(_ip, _port, _username, _password, _dbname);
            p->updateAliveTime(); // 更新开始空闲的起始时间
            _connectionQue.push(p);
            ++_connectionCnt;
        }

        cv.notify_all(); // 通知消费者线程可以消费了
    }
}

// 从连接池中获取一个可用的空闲连接接口，用完之后智能指针直接析构，放回连接池就好
std::shared_ptr<Connection> ConnectionPool::getConnection() {
    std::unique_lock<std::mutex> ulock(_queueMutex);
    while (_connectionQue.empty()) {
        if (std::cv_status::timeout == cv.wait_for(ulock, std::chrono::milliseconds(_connectionTimeout))) {
            if (_connectionQue.empty()) {
                LOG("get timeout...get connection failed!");
                return nullptr;
            }
        }
    }

    // 重写智能指针的析构器，防止其直接delete连接，让其析构变成归还到队列中
    std::shared_ptr<Connection> sptr(_connectionQue.front(),
        [&](Connection* pconn) {
            // 服务器应用线程中调用这个，所以要考虑线程安全
            std::unique_lock<std::mutex> ulock(_queueMutex);
            pconn->updateAliveTime(); // 更新开始空闲的起始时间
            _connectionQue.push(pconn);
        });

    _connectionQue.pop();
    cv.notify_all(); // 通知生产者线程检查一下队列是否空了，空了就要继续生产

    return sptr;
}

// 启动一个新的定时线程，扫描多余的空闲连接，超过maxIdleTime的连接进行回收
void ConnectionPool::scannerConnectionTask() {
    for (;;) {
        // 模拟定时效果
        std::this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));

        // 扫描整个队列，释放多余的连接
        std::unique_lock<std::mutex> ulock(_queueMutex);
        if (_connectionCnt > _initSzie) {
            Connection *p = _connectionQue.front();
            if (p->getAliveTime() >= (_maxIdleTime * 1000)) {
                _connectionQue.pop();
                delete p; // 释放连接
            } else {
                break; // 队头都没有超过，其他肯定没有
            }
        }
    }
}