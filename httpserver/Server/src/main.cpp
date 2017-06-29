#include "WXNotifyMgr.h"
#include "tinyxml.h"
#if _WIN32
#include <windows.h>
#else
#include "unistd.h"
#endif

int main()
{
	WXNotifyMgr::GetInstance()->run();
	while (true)
	{
		WXNotifyMgr::GetInstance()->Process();
#if _WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
	}
	return 0;
}