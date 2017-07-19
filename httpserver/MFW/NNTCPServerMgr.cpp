#include "NNTCPServerMgr.h"
#include <iostream>

std::map<int, std::shared_ptr<NNTCPServer>> NNTCPServerMgr::mNetServers;
std::map<int, std::shared_ptr<NNTCPClient>> NNTCPServerMgr::mNetClients;
std::mutex NNTCPServerMgr::mNetNodesMutex;
std::vector<uv_loop_t*> NNTCPServerMgr::loops;

void NNTCPNode::SendData(uv_stream_t * session, std::string data)
{
	write_req_t* wr = (write_req_t*)malloc(sizeof *wr);
	uv_buf_t buf = uv_buf_init((char*)data.data(), data.size());
	wr->buf = uv_buf_init(buf.base, data.size());
	session->data = (void*)&nNNodeInfo;
	uv_write(&wr->req, session, &wr->buf, 1, NNTCPServerMgr::WriteCb);
}
bool NNTCPNode::CloseSession(uv_stream_t * session)
{
	uv_close((uv_handle_t*)session, NNTCPServerMgr::CloseCb);
	return true;
}
void NNTCPNode::ReadCb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
	if (nread == UV_EOF || nread == UV_ECONNRESET) {
		if (OnDisConnected != nullptr)
		{
			OnDisConnected(client, *this);
		}
		uv_close((uv_handle_t*)client, NNTCPServerMgr::CloseCb);
		return;
	}
	buf->base[nread] = '\0';
	std::string recivedata(buf->base, nread);
	if (OnRead != nullptr)
	{
		OnRead(client, recivedata, *this);
	}
}
void NNTCPNode::CloseCb(uv_handle_t* handle)
{

}
void NNTCPNode::ConnectCb(uv_loop_t* loop, uv_stream_t *server, int status)
{
	if (status < 0) {
		return;
	}
	uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(loop, client);
	if (uv_accept(server, (uv_stream_t*)client) == 0) {
		client->data = server->data;
		uv_read_start((uv_stream_t*)client, NNTCPServerMgr::AllocBuffer, NNTCPServerMgr::ReadCb);
		if (OnConnected != nullptr)
		{
			OnConnected((uv_stream_t*)client, *this);
		}
	}
	else {
		uv_close((uv_handle_t*)client, NULL);
	}
}

void NNTCPNode::ConnectCbClient(uv_loop_t* loop, uv_connect_t* conn_req, int status)
{
	if (status < 0) {
		return;
	}
	int r = uv_read_start((uv_stream_t*)&server, NNTCPServerMgr::AllocBuffer, NNTCPServerMgr::ReadCb);
	if (OnConnected != nullptr)
		OnConnected((uv_stream_t*)&server, *this);
}

void NNTCPNode::WriteCb(uv_write_t* req, int status)
{

}
void NNTCPNode::TimerCb(uv_timer_t* handle)
{
	if (timerCB != nullptr)
	{
		timerCB(handle);
	}
}

void NNTCPServerMgr::AllocBuffer(uv_handle_t *h, size_t size, uv_buf_t *buf) {
	size = DEFAULT_BACKLOG;
	buf->base = (char*)malloc(size);
	buf->len = size;
}
void NNTCPServerMgr::ReadCb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	std::lock_guard<std::mutex> lg(mNetNodesMutex);
	NNNodeInfo node = *(NNNodeInfo*)client->data;
	if (!node.isClient)
	{
		std::shared_ptr<NNTCPServer> netNodePtr;
		if (mNetServers.find(node.Port) != mNetServers.end())
		{
			netNodePtr = mNetServers[node.Port];
		}
		if (netNodePtr == nullptr)
			return;
		netNodePtr->ReadCb(client, nread, buf);
	}
	else
	{
		std::shared_ptr<NNTCPClient> netNodePtr;
		if (mNetClients.find(node.Port) != mNetClients.end())
		{
			netNodePtr = mNetClients[node.Port];
		}
		if (netNodePtr == nullptr)
			return;
		netNodePtr->ReadCb(client, nread, buf);
	}
}

void NNTCPServerMgr::CloseCb(uv_handle_t* handle)
{
	std::lock_guard<std::mutex> lg(mNetNodesMutex);
	NNNodeInfo node = *(NNNodeInfo*)handle->data;
	if (!node.isClient)
	{
		std::shared_ptr<NNTCPServer> netNodePtr;
		if (mNetServers.find(node.Port) != mNetServers.end())
		{
			netNodePtr = mNetServers[node.Port];
		}
		if (netNodePtr == nullptr)
			return;
		netNodePtr->CloseCb(handle);
	}
	else
	{
		std::shared_ptr<NNTCPClient> netNodePtr;
		if (mNetClients.find(node.Port) != mNetClients.end())
		{
			netNodePtr = mNetClients[node.Port];
		}
		if (netNodePtr == nullptr)
			return;
		netNodePtr->CloseCb(handle);
	}
}
void NNTCPServerMgr::TimerCb(uv_timer_t* handle) {
	std::lock_guard<std::mutex> lg(mNetNodesMutex);
	NNNodeInfo node = *(NNNodeInfo*)handle->data;
	if (!node.isClient)
	{
		std::shared_ptr<NNTCPServer> netNodePtr;
		if (mNetServers.find(node.Port) != mNetServers.end())
		{
			netNodePtr = mNetServers[node.Port];
		}
		if (netNodePtr == nullptr)
			return;
		netNodePtr->TimerCb(handle);
	}
	else
	{
		std::shared_ptr<NNTCPClient> netNodePtr;
		if (mNetClients.find(node.Port) != mNetClients.end())
		{
			netNodePtr = mNetClients[node.Port];
		}
		if (netNodePtr == nullptr)
			return;
		netNodePtr->TimerCb(handle);
	}

}
void NNTCPServerMgr::ConnectCb(uv_stream_t *server, int status) {
	std::lock_guard<std::mutex> lg(mNetNodesMutex);
	std::shared_ptr<NNTCPServer> netNodePtr;
	NNNodeInfo node = *(NNNodeInfo*)server->data;
	if (!node.isClient)
	{
		if (mNetServers.find(node.Port) != mNetServers.end())
		{
			netNodePtr = mNetServers[node.Port];
		}
		if (netNodePtr == nullptr)
			return;
		netNodePtr->ConnectCb(server->loop, server, status);
	}
}
void NNTCPServerMgr::ConnectCbClient(uv_connect_t* conn_req, int status) {
	std::lock_guard<std::mutex> lg(mNetNodesMutex);
	NNNodeInfo node = *(NNNodeInfo*)conn_req->data;
	if (node.isClient)
	{
		std::shared_ptr<NNTCPClient> netNodePtr;
		if (mNetClients.find(node.Port) != mNetClients.end())
		{
			netNodePtr = mNetClients[node.Port];
		}
		if (netNodePtr == nullptr)
			return;
		netNodePtr->ConnectCbClient(conn_req->handle->loop, conn_req, status);
	}
}
void NNTCPServerMgr::WriteCb(uv_write_t* client, int status) {
	std::lock_guard<std::mutex> lg(mNetNodesMutex);
	NNNodeInfo node = *(NNNodeInfo*)client->handle->data;
	std::shared_ptr<NNTCPServer> netNodePtr;
	if (mNetServers.find(node.Port) != mNetServers.end())
	{
		netNodePtr = mNetServers[node.Port];
	}
	if (netNodePtr == nullptr)
		return;
	netNodePtr->WriteCb(client, status);
}

bool NNTCPServerMgr::RunServer(std::vector<NNNodeInfo>NNServerInfos)
{
	uv_loop_t* loop;
	loops.push_back(loop);
	loop = uv_loop_new();
	int r;
	for (int i = 0; i < NNServerInfos.size(); i++)
	{
		if (!NNServerInfos[i].isClient)
		{
			std::shared_ptr<NNTCPServer> NetSessionPtr(new NNTCPServer());
			NetSessionPtr->OnConnected = NNServerInfos[i].OnConnected;
			NetSessionPtr->OnDisConnected = NNServerInfos[i].OnDisConnected;
			NetSessionPtr->OnRead = NNServerInfos[i].OnRead;
			NetSessionPtr->nNNodeInfo = NNServerInfos[i];
			mNetNodesMutex.lock();
			if (mNetServers.find(NNServerInfos[i].Port) != mNetServers.end())
			{
				mNetNodesMutex.unlock();
				continue;
			}
			mNetNodesMutex.unlock();

			struct sockaddr_in addr;

			assert(0 == uv_ip4_addr(NNServerInfos[i].Ip.c_str(), NNServerInfos[i].Port, &addr));
			r = uv_tcp_init(loop, &NetSessionPtr->server);
			assert(r == 0);

			r = uv_tcp_bind(&NetSessionPtr->server, (const struct sockaddr*)&addr, 0);

			assert(r == 0);
			r = uv_listen((uv_stream_t*)&NetSessionPtr->server, DEFAULT_BACKLOG, NNTCPServerMgr::ConnectCb);
			if (r) {
				return 1;
			}
			NetSessionPtr->server.data = (void*)&NetSessionPtr->nNNodeInfo;
			NetSessionPtr->port = NNServerInfos[i].Port;
			NetSessionPtr->timerHandle.data = (void*)&NNServerInfos[i];
			r = uv_timer_init(loop, &NetSessionPtr->timerHandle);
			if (NNServerInfos[i].OnTimered != nullptr) {
				NetSessionPtr->timerCB = NNServerInfos[i].OnTimered;
			}
			r = uv_timer_start(&NetSessionPtr->timerHandle, NNTCPServerMgr::TimerCb, 1, 1);

			mNetNodesMutex.lock();
			mNetServers.insert(std::pair<int, std::shared_ptr<NNTCPServer>>(NNServerInfos[i].Port, NetSessionPtr));
			mNetNodesMutex.unlock();
		}
		else
		{
			std::shared_ptr<NNTCPClient> NetSessionPtr(new NNTCPClient());
			NetSessionPtr->OnConnected = NNServerInfos[i].OnConnected;
			NetSessionPtr->OnDisConnected = NNServerInfos[i].OnDisConnected;
			NetSessionPtr->OnRead = NNServerInfos[i].OnRead;
			NetSessionPtr->nNNodeInfo = NNServerInfos[i];
			mNetNodesMutex.lock();
			if (mNetClients.find(NNServerInfos[i].Port) != mNetClients.end())
			{
				mNetNodesMutex.unlock();
				continue;
			}
			mNetNodesMutex.unlock();

			struct sockaddr_in addr;

			assert(0 == uv_ip4_addr(NNServerInfos[i].Ip.c_str(), NNServerInfos[i].Port, &addr));
			r = uv_tcp_init(loop, &NetSessionPtr->server);
			assert(r == 0);

			r = uv_tcp_connect(&NetSessionPtr->con, &NetSessionPtr->server, (const struct sockaddr*)&addr, NNTCPServerMgr::ConnectCbClient);
			assert(r == 0);

			NetSessionPtr->server.data = (void*)&NetSessionPtr->nNNodeInfo;
			NetSessionPtr->con.data = (void*)&NetSessionPtr->nNNodeInfo;
			NetSessionPtr->port = NNServerInfos[i].Port;

			r = uv_timer_init(loop, &NetSessionPtr->timerHandle);
			NetSessionPtr->timerHandle.data = (void*)&NNServerInfos[i];
			if (NNServerInfos[i].OnTimered != nullptr) {
				NetSessionPtr->timerCB = NNServerInfos[i].OnTimered;
			}
			r = uv_timer_start(&NetSessionPtr->timerHandle, NNTCPServerMgr::TimerCb, 1, 1);

			mNetNodesMutex.lock();
			mNetClients.insert(std::pair<int, std::shared_ptr<NNTCPClient>>(NNServerInfos[i].Port, NetSessionPtr));
			mNetNodesMutex.unlock();

		}
	}

	return uv_run(loop, UV_RUN_DEFAULT);
}


