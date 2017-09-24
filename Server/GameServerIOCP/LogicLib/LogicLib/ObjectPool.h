#pragma once
#include <deque>
#include <vector>

namespace MDUtillity
{
	template<typename T>
	class ObjectPool
	{
	public:

		ObjectPool(int maxObjectNum)
			:_maxObjectNum{ maxObjectNum }
		{
			_pool.reserve(_maxObjectNum);

			for (int i = 0; i < _maxObjectNum; ++i)
			{
				auto newObject = new T();
				_pool.push_back(newObject);
				_freeIndex.push_back(i);

			}
		}

		~ObjectPool(){ Release(); }

		T& operator[](const int index)
		{
			return *_pool[index];
		}

		T* AllocObject()
		{
			if (_freeIndex.size() == 0)
			{
				return nullptr;
			}

			auto index = _freeIndex.front();

			return _pool[index];
			
		}

		int GetMaxObjectNum() { return _maxObjectNum; }

		bool FreeObject(int index)
		{
			_freeIndex.push_back(index);
		}

		void Release()
		{
			for (int i = 0; i < _maxObjectNum; ++i)
			{
				auto pool = _pool[i];
				_pool[i] = nullptr;
				delete pool;
			}
		}
	private:

		const int _maxObjectNum;

		std::vector<T*> _pool;
		std::deque<int> _freeIndex;
	};
}



