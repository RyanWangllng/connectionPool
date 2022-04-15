#include "connection.h"
using namespace std;



int main() {
    Connection conn;
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
            "wang youzeng", 24, "male");
    conn.connect("localhost", 3306, "root", "123456", "chat1");
    conn.update(sql);
    return 0;
}