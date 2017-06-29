#include "NetServer.h"
#include <iostream>
 uv_loop_t * NetServer::loop;
std::function<void() > NetServer::timerCB;
std::function<void(uv_stream_t * session, std::string data)> NetServer::OnRead;
std::function<void(uv_stream_t * session)> NetServer::OnConnected;
std::function<void(uv_stream_t * session)> NetServer::OnDisConnected;

void NetServer::AllocBuffer(uv_handle_t *h, size_t size, uv_buf_t *buf) {
	size = 1024;
	buf->base = (char*)malloc(size);
	buf->len = size;
}
uv_tcp_t NetServer::server;
void NetServer::ReadCb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	if (nread == UV_EOF || nread == UV_ECONNRESET) {
		if (OnDisConnected != nullptr)
		{
			OnDisConnected(client);
		}
		uv_close((uv_handle_t*)client, CloseCb);
		return;
	}
	buf->base[nread] = '\0';
	std::string recivedata(buf->base, nread);
	if (OnRead != nullptr)
	{
		OnRead(client, recivedata);
	}
}
void NetServer::CloseCb(uv_handle_t* handle)
{

}
/*定时器回掉*/
void NetServer::timer_cb(uv_timer_t* handle) {

	if (timerCB != nullptr)
	{
		timerCB();
	}

}
void NetServer::ConnectCb(uv_stream_t *server, int status) {
	if (status < 0) {
		// error!
		return;
	}
	uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(loop, client);
	if (uv_accept(server, (uv_stream_t*)client) == 0) {
		uv_read_start((uv_stream_t*)client, AllocBuffer, ReadCb);
		if (OnConnected != nullptr)
		{
			OnConnected((uv_stream_t*)client);
		}
	}

	else {
		uv_close((uv_handle_t*)client, NULL);
	}
}
int NetServer::RunServer(std::string ip, int port, std::function<void() > _timerCB, bool istimer, uint64_t timeout, uint64_t repeat)
{
	int r;

	struct sockaddr_in addr;
	loop = uv_default_loop();

	assert(0 == uv_ip4_addr(ip.c_str(), port, &addr));

	r = uv_tcp_init(loop, &server);
	assert(r == 0);

	r = uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
	assert(r == 0);
	r = uv_listen((uv_stream_t*)&server, DEFAULT_BACKLOG, ConnectCb);
	if (r) {

		return 1;
	}

	//定时器
	if (istimer)
	{
		//注册定时回调
		uv_timer_t once_timers;
		r = uv_timer_init(uv_default_loop(), &once_timers);
		if (_timerCB == nullptr)
			return 1;
		timerCB = _timerCB;
		//启动定时器
		r = uv_timer_start(&once_timers, timer_cb, timeout, repeat);
	}


	return uv_run(loop, UV_RUN_DEFAULT);
}
bool NetServer::CloseSession(uv_stream_t * session)
{
	uv_close((uv_handle_t*)session, CloseCb);
	return true;
}
void NetServer::WriteCb(uv_write_t* req, int status) {

}
void  NetServer::SendData(uv_stream_t * session, std::string data)
{
	write_req_t* wr = (write_req_t*)malloc(sizeof *wr);
	uv_buf_t buf = uv_buf_init((char*)data.data(), data.size());
	wr->buf = uv_buf_init(buf.base, data.size());
	uv_write(&wr->req, session, &wr->buf, 1, WriteCb);
}

