#include "SessionPool.h"
#include "Session.h"
namespace MDServerNetLib
{
	SessionPool::SessionPool(int maxSessionNum, int recvBufLen, int sendBufLen)
		:_maxObjectNum{ maxSessionNum }
	{
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
			//TODO:우선 세션이 사용가능한지 확인.
			if (_sessionPool[i]->_clntSocket == socket)
			{
				return _sessionPool[i];
			}
		}
		return nullptr;
	}

	Session * SessionPool::GetSessionByIndex(int index)
	{
		for (int i = 0; i < _maxObjectNum; ++i)
		{
			//TODO:우선 세션이 사용가능한지 확인.
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
				//뭔가 이상한 상황 비어있지 않은데 팝이 안되는 상황.
				//아마 없을것같다.
				return nullptr;
			}			
		}

		_allocatedObjectCount.fetch_add(1);
		return _sessionPool[sessionIndex];
	}

	bool SessionPool::SessionFree(Session * Session)
	{
		//TODO:세션풀에서 세션 초기화 
		//사용 불가능으로 만들고
		//초기화를 하고
		//프리 인덱스에 넣고
		//사용가능으로 바꾼다.
		//아마 세션에서 값을 확인할때 사용가능한 상태인지 계속 확인 해야 할듯.
		return false;
	}


}
