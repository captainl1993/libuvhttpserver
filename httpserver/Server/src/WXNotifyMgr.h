#pragma once
#include<queue>
#include "HttpMgr.h"
#include "Singleton.h"

class WXNotifyMgr :public Singleton<WXNotifyMgr>
{
private:
	std::queue<WXNotiFy> WXNotifys;
	std::mutex WXNotifysMutex;
	std::shared_ptr<std::thread> mHttpServerThread;
public:
	void pushWBMsg(WXNotiFy wbMsg);
	bool popWBMsg(WXNotiFy& wbMsg);
	void run();
	void Process();
};