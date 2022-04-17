#pragma once
#include <string>
#include <queue>
#include <chrono>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>
#include "connection.h"
#include "public.h"
/*
    连接池模块
*/

// 线程安全的单例模式，因为多个线程会共用这一个实例
class ConnectionPool {
public:
    // 获取连接池对象实例
    static ConnectionPool *getConnectionPool();
    // 从连接池中获取一个可用的空闲连接接口，用完之后智能指针直接析构，放回连接池就好
    std::shared_ptr<Connection> getConnection(); 

private:
    // 私有化构造函数
    ConnectionPool(); 
    // 从配置文件中加载配置项
    bool loadConfigFile(); 
    // 运行在独立的线程中，专门生产新连接
    void produceConnectionTask(); 
    // 启动一个新的定时线程，扫描多余的空闲连接，超过maxIdleTime的连接进行回收
    void scannerConnectionTask();

private:
    std::string _ip;        // mysql的IP地址
    unsigned short _port;   // mysql的端口号 3306
    std::string _username;  // mysql登录用户名
    std::string _password;  // mysql登录密码
    std::string _dbname;    // 数据库名称
    int _initSzie;          // 连接池的初始连接量
    int _maxSize;           // 连接池的最大连接量
    int _maxIdleTime;       // 连接池的最大空闲时间
    int _connectionTimeout; // 连接池获取连接的超时时间

    std::queue<Connection*> _connectionQue; // 存储mysql连接的队列
    std::mutex _queueMutex; // 维护连接队列的线程安全互斥锁
    std::atomic_int _connectionCnt; // 记录连接所创建的connection连接的总数量
    std::condition_variable cv; // 设置条件变量，用于连接生产线程和连接消费线程的通信
};