#include "connection.h"
#include "connectionPool.h"
using namespace std;



int main() {
    /*
    Connection conn;
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
            "wang youzeng", 24, "male");
    conn.connect("127.0.0.1", 3306, "root", "123456", "chat1");
    conn.update(sql);
    */
    Connection conn;
    conn.connect("127.0.0.1", 3306, "root", "123456", "chat1");

    clock_t begin = clock();
    
    std::thread t1([]() {
        // ConnectionPool *cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 500; ++i) {
            char sql[1024] = {0};
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                    "wang youzeng", 24, "male");
            // 不使用连接池
            Connection conn;
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat1");
            conn.update(sql);
            
            // 使用连接池
            // shared_ptr<Connection> sptr = cp->getConnection();
            // sptr->update(sql);
        }
    });
    std::thread t2([]() {
        // ConnectionPool *cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 500; ++i) {
            char sql[1024] = {0};
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                    "wang youzeng", 24, "male");
            // 不使用连接池
            Connection conn;
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat1");
            conn.update(sql);
            
            // 使用连接池
            // shared_ptr<Connection> sptr = cp->getConnection();
            // sptr->update(sql);
        }
    });
    /*
    std::thread t3([]() {
        // ConnectionPool *cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 2500; ++i) {
            char sql[1024] = {0};
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                    "wang youzeng", 24, "male");
            // 不使用连接池
            Connection conn;
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat1");
            conn.update(sql);
            
            // 使用连接池
            // shared_ptr<Connection> sptr = cp->getConnection();
            // sptr->update(sql);
        }
    });
    std::thread t4([]() {
        // ConnectionPool *cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 2500; ++i) {
            char sql[1024] = {0};
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                    "wang youzeng", 24, "male");
            // 不使用连接池
            Connection conn;
            conn.connect("127.0.0.1", 3306, "root", "123456", "chat1");
            conn.update(sql);
            
            // 使用连接池
            // shared_ptr<Connection> sptr = cp->getConnection();
            // sptr->update(sql);
        }
    });
    */
    t1.join();
    t2.join();
    // t3.join();
    // t4.join();

    clock_t end = clock();
    std::cout << (end - begin) << " ms" << std::endl;

#if 0
    for (int i = 0; i < 10000; ++i) {
        // 不使用连接池
        // Connection conn;
        // conn.connect("127.0.0.1", 3306, "root", "123456", "chat1");
        // conn.update(sql);
        
        // 使用连接池
        shared_ptr<Connection> sptr = cp->getConnection();
        sptr->update(sql);
    }
#endif

    return 0;
}