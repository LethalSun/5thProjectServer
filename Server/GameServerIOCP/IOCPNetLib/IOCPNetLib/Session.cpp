#include <memory>
#include "Session.h"
#include "SpinLockGuard.h"

namespace MDServerNetLib
{
	Session::Session(MDUtillity::LoggerBase* logger,int indexForPool, int recvBufLen, int sendBufLen)
		:_logger{ logger },
		_sendBuffer{sendBufLen},
		_recvBuffer{ recvBufLen },
		_maxRecvBufLen{ recvBufLen },
		_maxSendBufLen{ sendBufLen },
		index{ indexForPool }
	{
		InitializeCriticalSectionAndSpinCount(&_spinlockMutex, 4000);//4000 is recomended
	}

	Session::~Session()
	{
		DeleteCriticalSection(&_spinlockMutex);
	}

	bool Session::PostSendBuffer(PacketRaw & pkt)
	{
		if (!IsConnected())
		{
			return false;
		}

		MDUtillity::SpinLockGuard lockGuard(_spinlockMutex);

		auto asize = _sendBuffer.CheckAvailableSpaceSize();
		auto bsize = pkt._bodySize + PacketHeaderSize;
		_logger->Write(MDUtillity::LogType::INFO, "%s | SessionFunc Session pool available = %d, size = %d,IsSendable(%d)", __FUNCTION__, asize, bsize, _isSendAvailable.load());
		

		if (_sendBuffer.CheckAvailableSpaceSize() < pkt._bodySize + PacketHeaderSize)
		{
			return false;
		}
		_logger->Write(MDUtillity::LogType::INFO, "%s |SessionFunc Post Send ,IsSendable(%d)", __FUNCTION__, _isSendAvailable.load());


		auto dest = _sendBuffer.GetWritablePosition();
		auto header = PacketHeader{ pkt._packetId, pkt._bodySize };

		//TODO: 함수로 빼보자.
		memcpy_s(dest, _sendBuffer.CheckAvailableSpaceSize(), &header, PacketHeaderSize);
		memcpy_s(dest + PacketHeaderSize, _sendBuffer.CheckAvailableSpaceSize() - PacketHeaderSize, pkt._body.c_str(), pkt._bodySize);

		_sendBuffer.Commit(pkt._bodySize + PacketHeaderSize);
		_logger->Write(MDUtillity::LogType::INFO, "%s | SessionFunc Commit Send Size: %d ,IsSendable(%d)", __FUNCTION__ ,(int) pkt._bodySize + PacketHeaderSize, _isSendAvailable.load());


		return true;
	}

	bool Session::FlushSendBuffer()
	{
		if (!IsConnected())
		{
			_logger->Write(MDUtillity::LogType::INFO, "%s | SessionFunc now not connected", __FUNCTION__);
			return false;
		}


		MDUtillity::SpinLockGuard lockGuard(_spinlockMutex);

		if (_sendBuffer.GetContiguiousSize() == 0)
		{
			if (_isSendAvailable.load() == true)
			{
				return true;
			}
			return false;
		}

		_logger->Write(MDUtillity::LogType::INFO, "%s | SessionFunc len Not 0" __FUNCTION__);

		if (_isSendAvailable.load() == false)
		{
			_logger->Write(MDUtillity::LogType::INFO, "%s | SessionFunc isSendable false", __FUNCTION__);
			return false;
		}
		auto sendContext = new IOContext();

		ZeroMemory(&sendContext->overlapped, sizeof(WSAOVERLAPPED));
		ZeroMemory(&sendContext->wsaBuf, sizeof(WSABUF));

		sendContext->sessionIOtype = IOType::IO_SEND;
		sendContext->session = this;
		sendContext->wsaBuf.len = _sendBuffer.GetContiguiousSize();
		sendContext->wsaBuf.buf = _sendBuffer.GetbufferStartposition();

		DWORD sendBytes = 0;
		DWORD flag = 0;

		_logger->Write(MDUtillity::LogType::INFO, "%s | SessionFunc Send Start send length %d", __FUNCTION__, sendContext->wsaBuf.len);

		AddRef();

		if (SOCKET_ERROR == WSASend(_clntSocket,&(sendContext->wsaBuf),
			1,&sendBytes,flag,(LPWSAOVERLAPPED)sendContext,NULL))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				_logger->Write(MDUtillity::LogType::INFO, "%s |  SessionFunc WSASend Error Not Pending", __FUNCTION__);
				return false;
			}
		}
		
		

		SetSendable(false);
		
		_logger->Write(MDUtillity::LogType::INFO, "%s | SessionFunc Send reserved Size:%ud isSendable: %d", __FUNCTION__, sendContext->wsaBuf.len,_isSendAvailable.load());
		return true;
	}

	bool Session::PostRecv()
	{
		if (!IsConnected())
		{
			return false;
		}

		//리시브는 예약은 처음한번은 억셉트,그 뒤는 워커스레드가 하고 그 다음에  

		if (_recvBuffer.CheckAvailableSpaceSize() == 0)
		{
			return false;
		}

		auto recvContext = new IOContext();

		ZeroMemory(&recvContext->overlapped, sizeof(WSAOVERLAPPED));
		ZeroMemory(&recvContext->wsaBuf, sizeof(WSABUF));

		recvContext->sessionIOtype = IOType::IO_RECV;
		recvContext->session = this;
		recvContext->wsaBuf.buf = _recvBuffer.GetWritablePosition();
		recvContext->wsaBuf.len = _recvBuffer.CheckAvailableSpaceSize();

		DWORD recvbytes = 0;
		DWORD flags = 0;
		//리시브 예약,

		AddRef();
		if (SOCKET_ERROR == WSARecv(_clntSocket, &(recvContext->wsaBuf),
			1, &recvbytes, &flags, (LPWSAOVERLAPPED)recvContext, NULL))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				return false;
			}
		}

		return true;
	}

	void Session::CompleteSend(DWORD transferred)
	{
		if (!IsConnected()) {
			return;
		}

		MDUtillity::SpinLockGuard lockGuard(_spinlockMutex);

		_sendBuffer.Remove(transferred);

		SetSendable(true);

		_logger->Write(MDUtillity::LogType::INFO, "%s | SessionFunc Session %d IS Sendable(%d)", __FUNCTION__, index, _isSendAvailable.load());

	}

	void Session::CompleteRecv(DWORD transferred)
	{
		_recvBuffer.Commit(transferred);
	}

	void Session::AddRef()
	{
		_refCount.fetch_add(1);
	}

	void Session::ReleaseRef()
	{
		_refCount.fetch_sub(1);
	}

	int Session::GetRefCount()
	{
		return _refCount.load();
	}

	bool Session::Reset()
	{
		return release();
	}

	bool Session::release()
	{
		if (_refCount.load() != 0)
		{
			return false;
		}

		_clntSocket = INVALID_SOCKET;
		_refCount.store(0);
		_sendBuffer.Reset();
		_recvBuffer.Reset();
		SetConnected(false);
		SetSendable(false);
		return true;
	}

}