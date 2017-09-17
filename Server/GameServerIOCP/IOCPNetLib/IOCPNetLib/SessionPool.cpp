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
		//�������� Ǯ��ü�� �Ҹ� �Ǹ鼭 ������ �Ҹ��ڰ� ȣ��Ǿ �����.
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

	//TODO: �ʿ��� ���� ������?�̰� �� �̻��ѵ�?
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
				//���� �̻��� ��Ȳ ������� ������ ���� �ȵǴ� ��Ȳ.�Ƹ� �����Ͱ���.
				return nullptr;
			}			
		}

		_allocatedObjectCount.fetch_add(1);
		return _sessionPool[sessionIndex];
	}

	bool SessionPool::SessionFree(Session * session)
	{
		//TODO: ���ú갡 ���� �ǰų� ���尡 ���� �ɶ����� ���� ī���� ���� �Ǿ��ϰ�
		//TODO: ���ú곪 ���� ������ �ɶ����� ���� ī��带 ���� ���Ѿ� �Ѵ�.
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
