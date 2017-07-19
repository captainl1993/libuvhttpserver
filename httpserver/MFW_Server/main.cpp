#include "HttpMgr.h"
#include "Timer.h"
#if _WIN32
#include <windows.h>
#else
#include "unistd.h"
#endif
HttpMgr httpMgr;
void OnRead(uv_stream_t * session, HttpRequest& httpRequest, NNTCPNode& netNode)
{
	std::cout << httpRequest.GetUrl() << std::endl;
	std::map<std::string, std::string> params;
	httpMgr.SendData(session, "dsadasfs 佛fdsfsdfsd", params, netNode);
}
void OnConnected(uv_stream_t * session, NNTCPNode& netNode)
{
	std::cout << "Client Connected " << std::endl;
}
void OnDisConnected(uv_stream_t * session, NNTCPNode& netNode)
{
	std::cout << "Client DisConnected " << std::endl;
}
void OnTime()
{

}
long long sys_start = 0;
void OnTimerCB(Timer timer)
{
	std::cout << timer.mTick << std::endl; 
	long long sys_now = std::chrono::time_point_cast<std::chrono::milliseconds>(
		std::chrono::time_point<std::chrono::system_clock>::clock::now()).time_since_epoch().count();

	std::cout << sys_now << std::endl;
	std::cout << sys_now-sys_start << std::endl;
}
int main()
{
	std::vector<HttpStartInfo> ports;
	HttpStartInfo httpStartInfo;
	httpStartInfo.mIp = "127.0.0.1";
	httpStartInfo.mIsClient = false;
	httpStartInfo.mPort = 5555;
	ports.push_back(httpStartInfo);

	HttpStartInfo httpStartInfo1;
	httpStartInfo1.mIp = "127.0.0.1";
	httpStartInfo1.mIsClient = false;
	httpStartInfo1.mPort = 8888;
	ports.push_back(httpStartInfo1);

	HttpStartInfo httpStartInfo2;
	httpStartInfo2.mIp = "127.0.0.1";
	httpStartInfo2.mIsClient = false;
	httpStartInfo2.mPort = 9999;
	ports.push_back(httpStartInfo2);




	//定时器测试
	TimerMgr::GetInstance()->Run();
	long long timerTick = time(NULL) + 5;
	sys_start = time(NULL) * 1000;
	TimerMgr::GetInstance()->StartTimer(timerTick * 1000, OnTimerCB);
	TimerMgr::GetInstance()->StartTimer(timerTick * 1000 + 5000, OnTimerCB);
	TimerMgr::GetInstance()->StartTimer(timerTick * 1000 + 5000, OnTimerCB);

	std::thread net([&]() {
		httpMgr.OnConnectedPtr = OnConnected;
		httpMgr.OnDisConnectedPtr = OnDisConnected;
		httpMgr.OnReadServerPtr = OnRead;
		httpMgr.Start(ports);
	});

	while (true)
	{
		TimerMgr::GetInstance()->Process();
#if _WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
	}
	return 0;
}
