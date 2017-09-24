#pragma once
#include <vector>
#include <string>
namespace LogicLib
{
	class Match;

	enum class PlayerState
	{
		NOTUSED = -1,
		USED = 0,
		DEPLOY,
		MATCHING,
		PLAYING,
		ENDPLAYING,
	};

	class Player
	{
	public:
		Player() = default;
		~Player();

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

		bool Reset() 
		{
			_sessionIdx = 0;
			_deplyedTileCount = 0;
			_state = PlayerState::NOTUSED;
			_deployInfo.clear();
			_matchInfo = nullptr;
			_rating = 0;
			_winNum = 0;
			_loseNum = 0; 
			_isTurn = false;
			_id.clear();
		}

		bool SetSessionIdx(int sessionIdx)
		{
			_sessionIdx = sessionIdx;
		}

		int GetSessionIdx()
		{
			return _sessionIdx;
		}

		bool SetState(int state)
		{
			_state = (PlayerState)state;
		}

		bool SetDeployInfo(std::string deployInfo)
		{
			_deployInfo.clear();

			//TODO: ���� �� ��ġ ������ ������ ���� �ʰų� � �������� �ּ��� ������ �ٿ��� �ʴ´ٸ�?
			int i = 0;
			for (auto& info : deployInfo)
			{
				if (info == '0')
				{
					_deployInfo[i] = 0;
				}
				else
				{
					_deployInfo[i] = 1;
				}

				++i;
			}
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

		bool SetInfo(int rating, int win, int lose)
		{
			_rating = rating;
			_winNum = win;
			_loseNum = lose;
		}

		bool SetTurn(bool turn)
		{
			_isTurn = turn;
		}

		bool SetID(std::string id)
		{
			_id.clear();
			_id = id;
		}

	private:
		int _sessionIdx = 0;//��Ŷ�� ���� �Ǹ� �̰����� � �÷��̾����� ã�� �� �ִ�.
		int _deplyedTileCount = 0;

		PlayerState _state = PlayerState::NOTUSED;

		//TODO:Ŭ���̾�Ʈ���� �ϴ� �ùٸ� ��ġ�� �������� �ϰ� �÷��̾ �� ��ġ ������ ������ ��ȿ���� �Ǵ��Ѵ�.
		//�谡 ��ġ �Ǿ��ִ� Ÿ���� ���� ũ�⿡ ����� ü���� ������ ���� ��ũ�� ����Ʈ�εǾ��־ �ϳ��� Ÿ���� ü���� ���̸�
		//��ü ��ũ�� ����Ʈ�� ���鼭 ü���� ��´�. ü���� �ٱ��̸� ��Ƽ���� ��Ŷ�� Ư���� ǥ�ø� �ؼ� Ŭ���̾�Ʈ����
		//�ֺ� Ÿ���� �谡 ����Ÿ�Ϸ� Ȯ�� �Ҽ� �ְ� �Ѵ�.
		std::vector<int> _deployInfo;

		Match* _matchInfo = nullptr;

		int _rating = 0;
		int _winNum = 0;
		int _loseNum = 0;

		bool _isTurn = false;

		std::string _id;
	};
}


