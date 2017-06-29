#include "WXNotifyMgr.h"

void WXNotifyMgr::run()
{
	mHttpServerThread = std::shared_ptr< std::thread>(new std::thread([&]() {
		HttpMgr::Start();
	}));
}
void WXNotifyMgr::pushWBMsg(WXNotiFy wbMsg)
{
	std::lock_guard < std::mutex > lg(WXNotifysMutex);
	WXNotifys.push(wbMsg);
}
bool WXNotifyMgr::popWBMsg(WXNotiFy& wbMsg)
{
	std::lock_guard < std::mutex > lg(WXNotifysMutex);
	if (WXNotifys.size() == 0)
		return false;
	wbMsg = WXNotifys.front();
	WXNotifys.pop();
	return true;
}
void WXNotifyMgr::Process()
{
	//处理逻辑
	while (true)
	{
		WXNotiFy  wbMsg;
		if (popWBMsg(wbMsg))
		{
			//处理票据数据
			std::cout << wbMsg.appid << std::endl;
			std::cout << wbMsg.attach << std::endl;
			std::cout << wbMsg.bank_type<< std::endl;
			std::cout << wbMsg.fee_type << std::endl;
		}
		else
		{
			break;
		}
	}
}