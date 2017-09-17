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

			for (int i = 0; i < maxSessionNum; ++i)
			{
				auto newObject = new T();
				_pool.push_back();
				_freeindex.push_back(i);

			}
		}

		~ObjectPool(){ Release(); }

		T& operator[](const int index)
		{
			return _pool[index];
		}
	

		T* GetObjectByUID(int UID)
		{
			for (int i = 0; i < _maxObjectNum; ++i)
			{
				if (_pool[i]->IsUsed() == false)
				{
					return nullptr;
				}

				if (_pool[i]->_uID == socket)
				{
					return _pool[i];
				}
			}
			return nullptr;
		}

		T* AllocObject()
		{

		}

		bool FreeObject();

		void Release();

	private:
		void init();

	private:

		const int _maxObjectNum;

		std::vector<T*> _pool;
		std::deque<int> _freeIndex;
	};
}



