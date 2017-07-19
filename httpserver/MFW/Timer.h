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
	long long mId;	//ʱ��id
	long long mTick;	//��ʱ��
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
	std::map<long long, std::shared_ptr<Timer>, std::less<long long>> mIDTimers;//id��Ӧ
	std::multimap<long long, std::shared_ptr<Timer>,std::less<long long>> mTickTimers;//ʱ���Ӧ
	std::mutex mTimerMutex;

	void PushTimer(Timer& timer);
	bool PopTimer(Timer& timer);
public:
	void Run();
	void Process();
	long long  StartTimer(long long NextTick, std::function<void(Timer)> timerCB);
	bool CancelTimer(long long timerId);
};