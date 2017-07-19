#pragma once
#include <iostream>
#include <stdlib.h>
#include <memory>
#include "NNTCPServerMgr.h"
#include <string>
#include "HttpParser.h"
class HttpStartInfo
{
public:
	std::string mIp="0.0.0.0";
	int mPort = 5555;
	bool mIsClient = false;
};
class HttpMgr {
private:
	//��ȡ��������
	void OnRead(uv_stream_t * session, std::string data, NNTCPNode& netNode);
	//���ӽ���
	void OnConnected(uv_stream_t * session, NNTCPNode& netNode);
	//���ӶϿ�
	void OnDisConnected(uv_stream_t * session, NNTCPNode& netNode);
	//��ʱ������
	void OnTimer(uv_timer_t* handle);
public:
	std::function<void(uv_stream_t * session, HttpRequest& httpRequest, NNTCPNode& netNode)> OnReadServerPtr = nullptr;
	std::function<void(uv_stream_t * session, HttpResPonse& httpResPonse, NNTCPNode& netNode)> OnReadClientPtr = nullptr;
	std::function<void(uv_stream_t * session, NNTCPNode& netNode)> OnConnectedPtr = nullptr;
	std::function<void(uv_stream_t * session, NNTCPNode& netNode)> OnDisConnectedPtr = nullptr;
	std::function<void(uv_timer_t* handle)> OnTimerPtr = nullptr;
	//����
	void Start(std::vector<HttpStartInfo> startInfos);
	//�������ݣ����ر�����
	void SendDataClient(uv_stream_t * session, std::string data, NNTCPNode& netNode, std::map<std::string, std::string> params, std::string Url);
	void SendData(uv_stream_t * session, std::string data, std::map<std::string, std::string> params, NNTCPNode& netNode);
};