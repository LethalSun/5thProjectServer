#pragma once
//#include "pch.h"

namespace MDUtillity
{

	class CircularBuffer
	{
	public:
		CircularBuffer(size_t capacity);
		~CircularBuffer();

		void Reset();

		size_t CheckAvailableSpaceSize();

		void Remove(size_t len);

		size_t GetWritedSize()const;

		size_t GetContiguiousSize()const;

		char* GetWritablePosition()const;

		void Commit(size_t length);

		char* GetbufferStartposition()const;
	private:
		void allocateSecondaryRegion();

		size_t getPrimaryRegionAvailableSpaceSize();

		size_t getBeforePrimaryRegionSpaceSize();

		size_t getSecondaryRegionAvailableSpaceSize();


	private:

		char * _buffer;
		char * _bufferEnd;

		char * _primaryRegion;
		size_t _primaryRegionSize;

		char* _secondaryRegion;
		size_t _secondaryRegionSize;

		size_t _capacity;
	};
}

