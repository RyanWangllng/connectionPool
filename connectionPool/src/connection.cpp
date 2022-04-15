#include "public.h"
#include "connection.h"


Connection::Connection() {
    // 初始化数据库连接
    mysql_init(&_conn);
    // _conn = mysql_init(nullptr);
}

Connection::~Connection() {
    // 释放数据库连接资源
    if (&_conn) {
        mysql_close(&_conn);
    }
}

bool Connection::connect(std::string ip, unsigned short port, std::string user, std::string password, std::string dbname) {
    // 连接数据库
    MYSQL *p = mysql_real_connect(&_conn, ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
    if (p != nullptr) {
        LOG("CONNECT SUCCESS!");
        return true;
    } else {
        LOG(mysql_error(p)); // 连接不成功时打印信息
        LOG(mysql_errno(p));
        LOG("CONNECT FAILED!");
        return false;
    }
}

bool Connection::update(std::string sql) {
    if (mysql_query(&_conn, sql.c_str())) {
        LOG("更新失败：" + sql);
        return false;
    }
    return true;
}

MYSQL_RES* Connection::query(std::string sql) {
    if (mysql_query(&_conn, sql.c_str())) {
        LOG("查询失败：" + sql);
        return nullptr;
    }
    return mysql_use_result(&_conn);
}