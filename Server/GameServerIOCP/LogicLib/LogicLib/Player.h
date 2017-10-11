#pragma once
#include <vector>
#include <string>
namespace LogicLib
{
	class Match;

	enum class PlayerState
	{
		NOTUSED = -1,
		DEPLOY,
		MATCHING,
		PLAYING,
	};

	class Player
	{
	public:
		Player() = default;
		~Player();
		void InitIndex(int idx) { _indexInPool = idx; }

		int GetIndex() { return _indexInPool; }

		int GetSessionIdx() { return _sessionIdx; }

		bool IsUsed() 
		{
			if (_state == PlayerState::NOTUSED)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		bool IsDeploy()
		{
			if (_state == PlayerState::DEPLOY)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool IsMatching()
		{
			if (_state == PlayerState::MATCHING)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool IsPlaying();


		bool Reset() 
		{
			_sessionIdx = 0;
			_deplyedTileCount = 0;
			_maxdeplyedTileCount = 0;
			_state = PlayerState::NOTUSED;
			_deployInfo.clear();
			_matchInfo = nullptr;
			_rating = 0;
			_winNum = 0;
			_loseNum = 0; 
			_id.clear();
			return true;
		}

		bool SetSessionIdx(int sessionIdx)
		{
			_sessionIdx = sessionIdx;
			return true;
		}

		bool SetState(int state)
		{
			_state = (PlayerState)state;
			return true;
		}

		bool SetDeployInfo(std::string deployInfo)
		{
			_deployInfo.clear();

			//TODO: ���� �� ��ġ ������ ������ ���� �ʰų� � �������� �ּ��� ������ �ٿ��� �ʴ´ٸ�?

			int deployTileCount = 0;

			for (int i = 0; i < deployInfo.length(); ++i)
			{
				//�׳� '0'�� ���� �����ص� ������ �ϴ� �׳� �� �Ѵ�.
				if (deployInfo[i] == '0')
				{
					_deployInfo.push_back(0);
				}
				else if (deployInfo[i] = '1')
				{
					_deployInfo.push_back(1);
					++deployTileCount;
				}
				else
				{
					return false;
				}

			}

			_allTileCount = deployInfo.length();
			_maxdeplyedTileCount =_deplyedTileCount = deployTileCount;
			return true;
		}

		bool ClearDeployInfo()
		{
			_deployInfo.clear();
			_maxdeplyedTileCount = 0;
			_deplyedTileCount = 0;
			return true;
		}

		int GetMaxHp() { return _maxdeplyedTileCount; }

		int GetHp() { return _deplyedTileCount; }

		bool SetHp(int hp) { _deplyedTileCount = hp;}

		bool IsHPZero()
		{
			if (_deplyedTileCount <= 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool IsDeployCorrect()
		{
			if (_allTileCount == 0)
			{
				return false;
			}

			if (_allTileCount != _deployInfo.size())
			{
				return false;
			}

			return true;
		}

		bool SetMatchInfo(Match* match)
		{
			if (_matchInfo == nullptr)
			{
				_matchInfo = match;
				return true;
			}
			else
			{
				return false;
			}

		}

		Match* GetMatchInfo() { return _matchInfo; }

		bool Bomb(int idx);


		int GetTileInfo(int idx)
		{
			return _deployInfo[idx];
		}

		bool Hitted(int idx)
		{
			_deployInfo[idx] = -1;
			_deplyedTileCount -= 1;
			return true;
		}
		bool SetInfo(int rating, int win, int lose)
		{
			_rating = rating;
			_winNum = win;
			_loseNum = lose;
			return true;
		}

		bool IsMyTurn();


		bool SetID(std::string id)
		{
			_id.clear();
			_id = id;
			return true;
		}
		
		Player* GetEnemy();


	private:
		int _indexInPool = 0;
		int _sessionIdx = 0;//��Ŷ�� ���� �Ǹ� �̰����� � �÷��̾����� ã�� �� �ִ�.


		PlayerState _state = PlayerState::NOTUSED;

		//TODO:Ŭ���̾�Ʈ���� �ϴ� �ùٸ� ��ġ�� �������� �ϰ� �÷��̾ �� ��ġ ������ ������ ��ȿ���� �Ǵ��Ѵ�.
		//�谡 ��ġ �Ǿ��ִ� Ÿ���� ���� ũ�⿡ ����� ü���� ������ ���� ��ũ�� ����Ʈ�εǾ��־ �ϳ��� Ÿ���� ü���� ���̸�
		//��ü ��ũ�� ����Ʈ�� ���鼭 ü���� ��´�. ü���� �ٱ��̸� ��Ƽ���� ��Ŷ�� Ư���� ǥ�ø� �ؼ� Ŭ���̾�Ʈ����
		//�ֺ� Ÿ���� �谡 ����Ÿ�Ϸ� Ȯ�� �Ҽ� �ְ� �Ѵ�.
		int _maxdeplyedTileCount = 0;
		int _deplyedTileCount = 0;//�̰� ü���� �ǹ� �Ѵ�.
		int _allTileCount = 0;
		std::vector<int> _deployInfo;
		Match* _matchInfo = nullptr;

		int _rating = 0;
		int _winNum = 0;
		int _loseNum = 0;

		std::string _id;
	};
}


