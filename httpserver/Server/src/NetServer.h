#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <uv.h>
#include <memory>
#include <thread>
#include <functional>

#define DEFAULT_BACKLOG 10240
typedef struct {
	uv_write_t req;
	uv_buf_t buf;
} write_req_t;

//网络基础库
class NetServer
{
private:
	static uv_loop_t *loop;
	static uv_tcp_t server;
	//定时器
	static uv_timer_t timerHandle;
	//定时器的返回
	static std::function<void() > timerCB;
public:
	//读取连接数据
	static std::function<void(uv_stream_t * session, std::string data)> OnRead;
	//连接进入
	static std::function<void(uv_stream_t * session)> OnConnected;
	//连接断开
	static std::function<void(uv_stream_t * session)> OnDisConnected;
	//启动网络
	static int RunServer(std::string ip, int port, std::function<void() > _timerCB, bool istimer = false, uint64_t timeout = 0, uint64_t repeat = 0);
	//如果Session不存在返回false ，如果存在返回true
	static bool CloseSession(uv_stream_t * session);
	static void AllocBuffer(uv_handle_t *h, size_t size, uv_buf_t *buf);
	static void ReadCb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
	static void CloseCb(uv_handle_t* handle);
	static void ConnectCb(uv_stream_t *server, int status);
	static void WriteCb(uv_write_t* req, int status);
	static void timer_cb(uv_timer_t* handle);
	//发送数据
    static void  SendData(uv_stream_t *session, std::string data);
};
