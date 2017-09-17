#include "SessionPool.h"
#include "Session.h"
namespace MDServerNetLib
{
	SessionPool::SessionPool(int maxSessionNum, int recvBufLen, int sendBufLen)
		:_maxObjectNum{ maxSessionNum }
	{
		_sessionPool.reserve(_maxObjectNum);
		for (int i = 0; i < maxSessionNum; ++i)
		{
			auto newSession = new Session(i, recvBufLen, sendBufLen);
			_sessionPool.push_back(newSession);
			_freeindex.push(i);

		}
	}

	SessionPool::~SessionPool()
	{
		for (int i = 0; i < _maxObjectNum; ++i)
		{
			auto session = _sessionPool[i];
			_sessionPool[i] = nullptr;
			delete session;
		}
		//나머지는 풀객체가 소멸 되면서 각각의 소멸자가 호출되어서 사라짐.
	}

	Session * SessionPool::GetSeesionBySocketFD(SOCKET socket)
	{
		for (int i = 0; i < _maxObjectNum; ++i)
		{
			if (_sessionPool[i]->IsConnected() == false)
			{
				return nullptr;
			}

			if (_sessionPool[i]->_clntSocket == socket)
			{
				return _sessionPool[i];
			}
		}
		return nullptr;
	}

	//TODO: 필요한 일이 있을까?이거 좀 이상한데?
	Session * SessionPool::GetSessionByIndex(int index)
	{
		for (int i = 0; i < _maxObjectNum; ++i)
		{
			if (_sessionPool[i]->IsConnected() == false)
			{
				return nullptr;
			}

			if (_sessionPool[i]->index == index)
			{
				return _sessionPool[i];
			}
		}
		return nullptr;
	}

	Session * SessionPool::AllocateSession()
	{
		int sessionIndex;

		if (_freeindex.try_pop(sessionIndex) == false)
		{
			if (_freeindex.empty() == true)
			{
				return nullptr;
			}
			else
			{
				//뭔가 이상한 상황 비어있지 않은데 팝이 안되는 상황.아마 없을것같다.
				return nullptr;
			}			
		}

		_allocatedObjectCount.fetch_add(1);
		return _sessionPool[sessionIndex];
	}

	bool SessionPool::SessionFree(Session * session)
	{
		//TODO: 리시브가 종료 되거나 센드가 종료 될때마다 레프 카운드는 감소 되야하고
		//TODO: 리시브나 센드 예약을 걸때마다 레프 카운드를 증가 시켜야 한다.
		session->SetConnected(false);

		if (session->GetRefCount() <= 0)
		{
			sessionFree(session);
		}
		else
		{
			return false;
		}
		return true;
	}

	bool SessionPool::sessionFree(Session * Session)
	{
		Session->Reset();
		_freeindex.push(Session->index);
		return false;
	}


}
