#pragma once
#include <Windows.h>

namespace MDUtillity
{
	class SpinLockGuard
	{
	public:
		SpinLockGuard(CRITICAL_SECTION& lock)
			:_lock{ lock }
		{
			EnterCriticalSection(&_lock);
		}
		~SpinLockGuard()
		{
			LeaveCriticalSection(&_lock);
		}
	private:

		CRITICAL_SECTION _lock;
	};

}

