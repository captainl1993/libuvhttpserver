#include "Timer.h"
#include <chrono>
#if _WIN32
#include <windows.h>
#else
#include "unistd.h"
#endif
void Timer::OnTimerCB()
{
	if (OnTimerPtr != nullptr)
	{
		OnTimerPtr(*this);
	}
}

void TimerMgr::TimerRun()
{
	while (true)
	{
		std::lock_guard<std::mutex> lg(mTimerMutex);
		long long sys_now = std::chrono::time_point_cast<std::chrono::milliseconds>(
			std::chrono::time_point<std::chrono::system_clock>::clock::now()).time_since_epoch().count();
		for (auto itr = mTickTimers.begin(); itr != mTickTimers.end(); )
		{
			if (itr->second->mTick <= sys_now)
			{
				Timer timer = *itr->second;
				PushTimer(timer);
				mIDTimers.erase(timer.mId);
				mTickTimers.erase(itr++);
			}
			else
			{
				break;
			}
		}
#if _WIN32
		Sleep(1);
#else
		usleep(1000)
#endif
	}

}

void TimerMgr::Run()
{
	mThreadPtr = std::shared_ptr<std::thread>(new std::thread(std::bind(&TimerMgr::TimerRun, this)));
}

void TimerMgr::PushTimer(Timer& timer)
{
	std::lock_guard<std::mutex> lg(mFinishedTimerMutex);
	mFinishedTimers.push(timer);
}

bool TimerMgr::PopTimer(Timer& timer)
{
	std::lock_guard<std::mutex> lg(mFinishedTimerMutex);
	if (mFinishedTimers.size() == 0)
		return false;
	timer = mFinishedTimers.front();
	mFinishedTimers.pop();
	return true;
}

void TimerMgr::Process()
{
	Timer timer;
	while (PopTimer(timer))
	{
		timer.OnTimerCB();
	}
}


long long  TimerMgr::StartTimer(long long NextTick, std::function<void(Timer)> timerCB)
{
	std::lock_guard<std::mutex> lg(mTimerMutex);
	std::shared_ptr<Timer> timer(new Timer());
	if (mTimerId > 10000000000)
		mTimerId = 0;
	timer->mId = mTimerId++;
	timer->mTick = NextTick;
	timer->OnTimerPtr = timerCB;
	mIDTimers.insert(std::pair<long long, std::shared_ptr<Timer>>(timer->mId, timer));
	mTickTimers.insert(std::pair<long long, std::shared_ptr<Timer>>(NextTick, timer));
	return mTimerId;
}

bool TimerMgr::CancelTimer(long long timerId)
{
	std::lock_guard<std::mutex> lg(mTimerMutex);
	if (mIDTimers.find(timerId) != mIDTimers.end())
	{
		auto itr = mTickTimers.find(mIDTimers[timerId]->mTick);
		for (; itr != mTickTimers.end(); itr++)
		{
			if (itr->second->mId == timerId)
			{
				mTickTimers.erase(itr);
				break;
			}
			if (itr->second->mTick != mIDTimers[timerId]->mTick)
			{
				break;
			}
		}
		mIDTimers.erase(timerId);
		return true;
	}
	return false;
}