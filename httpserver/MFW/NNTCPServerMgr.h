#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "uv.h"
#include <memory>
#include <thread>
#include <functional>
#include <map>
#include <mutex>
#include <vector>
#include <queue>


class NNMsg;
class NNNodeInfo;
class NNTCPNode;

#define DEFAULT_BACKLOG 10240

typedef struct {
	uv_write_t req;
	uv_buf_t buf;
} write_req_t;


class  NNNodeInfo
{
public:
	std::string Ip = "0.0.0.0";
	int Port = 0;
	bool isClient = true;
	std::function<void(uv_stream_t * session, std::string data, NNTCPNode& netNode)> OnRead;
	std::function<void(uv_stream_t * session, NNTCPNode& netNode)> OnConnected = nullptr;
	std::function<void(uv_stream_t * session, NNTCPNode& netNode)> OnDisConnected = nullptr;
	std::function<void(uv_timer_t* handle) > OnTimered = nullptr;
};

class NNTCPNode
{
public:
	int port = 0;
	uv_tcp_t server;
	uv_timer_t timerHandle;
	NNNodeInfo nNNodeInfo;
	std::function<void(uv_timer_t* handle) > timerCB;
	std::function<void(uv_stream_t * session, std::string data, NNTCPNode& netNode)> OnRead;
	std::function<void(uv_stream_t * session, NNTCPNode& netNode)> OnConnected;
	std::function<void(uv_stream_t * session, NNTCPNode& netNode)> OnDisConnected;
	void  SendData(uv_stream_t *session, std::string data);
	bool CloseSession(uv_stream_t * session);
	void ReadCb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
	void CloseCb(uv_handle_t* handle);
	void ConnectCb(uv_loop_t* loop, uv_stream_t *server, int status);
	void ConnectCbClient(uv_loop_t* loop, uv_connect_t* conn_req, int status);
	void WriteCb(uv_write_t* req, int status);
	void TimerCb(uv_timer_t* handle);
};

class NNTCPServer:public NNTCPNode
{

};

class NNTCPClient :public NNTCPNode
{
public:
	std::string Ip = "0.0.0.0";
	uv_connect_t con;
};

class NNTCPServerMgr
{
private:
	static std::vector<uv_loop_t*> loops;
	static std::mutex mNetNodesMutex;
	static std::map<int, std::shared_ptr<NNTCPServer>> mNetServers;
	static std::map<int, std::shared_ptr<NNTCPClient>> mNetClients;
public:
	static bool RunServer(std::vector<NNNodeInfo> NNServerInfos);
	static void AllocBuffer(uv_handle_t *h, size_t size, uv_buf_t *buf);
	static void ReadCb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
	static void CloseCb(uv_handle_t* handle);
	static void ConnectCb(uv_stream_t *server, int status);
	static void WriteCb(uv_write_t* client, int status);
	static void TimerCb(uv_timer_t* handle);

	static void ConnectCbClient(uv_connect_t* conn_req, int status);
};
