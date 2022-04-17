### 数据库连接池
#### 使用技术：
- MySQL数据库
- C++11、多线程、智能指针
- 线程安全单例模式
- 生产者-消费者模型
#### 编译：
```Bash
git clone git@github.com:RyanWangllng/connectionPool.git

cd connectionPool/connectionPool/build
cmake ..
make
```
#### 运行：
```Bash
cd connectionPool/connectionPool/bin
./MySQLconnection
```
#### 具体实现：
1. 连接池只产生一个实例
2. 空闲的连接维护在一个线程安全的队列中
3. 连接池容纳的连接数有上线maxSize
4. 池中的连接空闲时间超过maxIdleTime就会被释放掉，只保留初始时候的连接个数
5. 设置连接超时，请求回去连接超过该时间就提示连接失败
#### 压力测试：
> 测试环境：Ubuntu 20.04 LTS、MySQL 8.0.28  
配置：腾讯云轻量应用服务器 2核 4G

|数据量|单线程使用连接池提升|四线程使用连接池提升|
|:-:|:-:|:-:|  
|1000 |92% |93%|
|5000|92%|94%|
|10000|92%|94%|