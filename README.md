# libuvhttpserver
httpserver base on libuv-tcp and http_parser

MFW框架，旨在基于libuv，构建高效的数时事通讯服务器框架。实现多并发、模块化、通用化
V0.0.1目前实现功能：
1、多线程多端口的tcp服务器及客户端
2、高性能http服务器框架，和客户端框架
3、基于C++11 实现的高效异步微秒级定时器
4、http_parser的C++面向对象封装

后续将继续实现：
1、基于mysql+redis+内存的多级数据缓存和持久化方案
2、tcp数据组包、拆包、黏包处理
3、针对unity3D的客户端连接，针对unreal engine的对接等
4、高效的消息处理分发机制
5、玩家状态控制--状态机


