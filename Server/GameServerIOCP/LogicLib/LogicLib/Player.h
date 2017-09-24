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

			//TODO: 만약 배 배치 정보가 완전히 오지 않거나 어떤 이유에서 최소한 갯수가 다오지 않는다면?
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

		bool SetInfo(int rating, int win, int lose)
		{
			_rating = rating;
			_winNum = win;
			_loseNum = lose;
			return true;
		}

		bool SetTurn(bool turn)
		{
			_isTurn = turn;
			return true;
		}

		bool SetID(std::string id)
		{
			_id.clear();
			_id = id;
			return true;
		}

	private:
		int _sessionIdx = 0;//패킷이 오게 되면 이것으로 어떤 플레이어인지 찾을 수 있다.
		int _deplyedTileCount = 0;

		PlayerState _state = PlayerState::NOTUSED;

		//TODO:클라이언트에서 일단 올바른 배치만 보내도록 하고 플레이어가 배 배치 정보를 받을때 유효성을 판단한다.
		//배가 배치 되어있는 타일은 배의 크기에 비례한 체력을 가지고 원형 링크드 리스트로되어있어서 하나의 타일의 체력이 깍이면
		//전체 링크드 리스트롤 돌면서 체력을 깍는다. 체력이 다깍이면 노티파이 패킷에 특별한 표시를 해서 클라이언트에서
		//주변 타일을 배가 없는타일로 확정 할수 있게 한다.
		std::vector<int> _deployInfo;

		Match* _matchInfo = nullptr;

		int _rating = 0;
		int _winNum = 0;
		int _loseNum = 0;

		bool _isTurn = false;

		std::string _id;
	};
}


