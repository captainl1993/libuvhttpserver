#pragma once
#include "Singleton.h"
#include <map>
#include <thread>
#include <stack>
#include <functional>
#include <queue>
class Timer
{
public:
	long long mId;	//时间id
	long long mTick;	//定时点
	void OnTimerCB();
	std::function<void(Timer)> OnTimerPtr;
};

class TimerMgr:public Singleton<TimerMgr>
{
private:
	std::mutex mFinishedTimerMutex;
	std::queue<Timer> mFinishedTimers;
	long long mTimerId = 0;
	std::shared_ptr<std::thread> mThreadPtr;
	void TimerRun();
	std::map<long long, std::shared_ptr<Timer>, std::less<long long>> mIDTimers;//id对应
	std::multimap<long long, std::shared_ptr<Timer>,std::less<long long>> mTickTimers;//时间对应
	std::mutex mTimerMutex;

	void PushTimer(Timer& timer);
	bool PopTimer(Timer& timer);
public:
	void Run();
	void Process();
	long long  StartTimer(long long NextTick, std::function<void(Timer)> timerCB);
	bool CancelTimer(long long timerId);
};