#include "CircularBuffer.h"
#include <iostream>

namespace MDUtillity
{

	CircularBuffer::CircularBuffer(size_t capacity) :
		_buffer{ new char[capacity] },
		_bufferEnd{ _buffer + capacity },
		_primaryRegion{ _buffer },
		_primaryRegionSize{ 0 },
		_secondaryRegion{ nullptr },
		_secondaryRegionSize{ 0 },
		_capacity{ capacity }
	{
	}

	CircularBuffer::~CircularBuffer()
	{
		delete[] _buffer;
		_buffer = nullptr;
		_bufferEnd = nullptr;
		_primaryRegion = nullptr;
		_secondaryRegion = nullptr;
		_primaryRegionSize = 0;
		_secondaryRegionSize = 0;
		_capacity = 0;
	}

	void CircularBuffer::Reset()
	{
		_primaryRegion = _buffer;
		_secondaryRegion = nullptr;
		_primaryRegionSize = 0;
		_secondaryRegionSize = 0;
		_bufferEnd = _buffer + _capacity;

		std::memset(_buffer, 0, _capacity);


	}

	void CircularBuffer::allocateSecondaryRegion()
	{
		_secondaryRegion = _buffer;
	}

	size_t CircularBuffer::getPrimaryRegionAvailableSpaceSize()
	{
		return (_bufferEnd - _primaryRegion - _primaryRegionSize);
	}

	size_t CircularBuffer::getBeforePrimaryRegionSpaceSize()
	{
		return (_primaryRegion - _buffer);
	}

	size_t CircularBuffer::getSecondaryRegionAvailableSpaceSize()
	{
		if (_secondaryRegion == nullptr)
		{
			return 0;
		}
		else
		{
			return (_primaryRegion - _secondaryRegion - _secondaryRegionSize);
		}
	}

	size_t CircularBuffer::CheckAvailableSpaceSize()
	{
		if (_secondaryRegion != nullptr)
		{
			return getSecondaryRegionAvailableSpaceSize();
		}
		else
		{
			if (getBeforePrimaryRegionSpaceSize() > getPrimaryRegionAvailableSpaceSize())
			{
				allocateSecondaryRegion();
				return getSecondaryRegionAvailableSpaceSize();
			}
			else
			{
				return getPrimaryRegionAvailableSpaceSize();
			}
		}
	}

	void CircularBuffer::Remove(size_t len)
	{
		size_t cnt = len;

		if (_primaryRegionSize > 0)
		{
			size_t aRemove = (cnt > _primaryRegionSize) ? _primaryRegionSize : cnt;
			_primaryRegionSize -= aRemove;
			_primaryRegion += aRemove;
			cnt -= aRemove;
		}

		if (cnt > 0 && _secondaryRegionSize > 0)
		{
			size_t bRemove = (cnt > _secondaryRegionSize) ? _secondaryRegionSize : cnt;
			_secondaryRegionSize -= bRemove;
			_secondaryRegion += bRemove;
			cnt -= bRemove;
		}

		if (_primaryRegionSize == 0)
		{
			if (_secondaryRegionSize > 0)
			{
				/// 앞으로 당겨 붙이기
				if (_secondaryRegion != _buffer)
					memmove(_buffer, _secondaryRegion, _secondaryRegionSize);

				_primaryRegion = _buffer;
				_primaryRegionSize = _secondaryRegionSize;
				_secondaryRegion = nullptr;
				_secondaryRegionSize = 0;
			}
			else
			{
				_secondaryRegion = nullptr;
				_secondaryRegionSize = 0;
				_primaryRegion = _buffer;
				_primaryRegionSize = 0;
			}
		}
	}

	size_t CircularBuffer::GetWritedSize()const
	{
		return _primaryRegionSize + _secondaryRegionSize;
	}

	size_t CircularBuffer::GetContiguiousSize()const
	{
		if (_primaryRegionSize > 0)
		{
			return _primaryRegionSize;
		}
		else
		{
			return _secondaryRegionSize;
		}
	}

	char * CircularBuffer::GetWritablePosition()const
	{
		if (_secondaryRegion != nullptr)
		{
			return (_secondaryRegion + _secondaryRegionSize);
		}
		else
		{
			return (_primaryRegion + _primaryRegionSize);
		}
	}

	void CircularBuffer::Commit(size_t length)
	{
		if (_secondaryRegion != nullptr)
		{
			_secondaryRegionSize += length;
		}
		else
		{
			_primaryRegionSize += length;
		}
	}

	char * CircularBuffer::GetbufferStartposition() const
	{
		if (_primaryRegionSize > 0)
		{
			return _primaryRegion;
		}
		else
		{
			return _secondaryRegion;
		}
	}

}