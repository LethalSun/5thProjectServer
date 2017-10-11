#include "pch.h"
#include "Match.h"
#include "Player.h"
#include <random>
#include <chrono>
namespace LogicLib
{

	Match::~Match()
	{
	}

	bool Match::SetPlayers(Player * player1, Player * player2)
	{
		if (getRandomNumber() % 2 == 0)
		{
			_attackPlayer = player1;
			_defencePlayer = player2;
		}
		else
		{
			_attackPlayer = player2;
			_defencePlayer = player1;
		}


		if (_attackPlayer->SetMatchInfo(this) == false ||
			_defencePlayer->SetMatchInfo(this) == false)
		{
			return false;
		}

		_attackPlayer->SetState(static_cast<int>(PlayerState::PLAYING));
		_defencePlayer->SetState(static_cast<int>(PlayerState::PLAYING));

		_isUsed = true;

		return true;
	}

	bool Match::Reset()
	{
		_isUsed = false;

		_attackPlayer->SetMatchInfo(nullptr);
		_defencePlayer->SetMatchInfo(nullptr);

		_attackPlayer->SetState(static_cast<int>(PlayerState::DEPLOY));
		_defencePlayer->SetState(static_cast<int>(PlayerState::DEPLOY));

		_attackPlayer->ClearDeployInfo();
		_defencePlayer->ClearDeployInfo();

		_attackPlayer = nullptr;
		_defencePlayer = nullptr;

		return true;
	}
	bool Match::changeTurn()
	{
		auto temp = _attackPlayer;
		_attackPlayer = _defencePlayer;
		_defencePlayer = temp;
		return true;
	}

	bool Match::IsMatchCorrect()
	{
		if (_isUsed == false)
		{
			return false;
		}

		if (_attackPlayer == nullptr ||
			_defencePlayer == nullptr)
		{
			return false;
		}

		if (_attackPlayer->GetMatchInfo() != this ||
			_defencePlayer->GetMatchInfo() != this)
		{
			return false;
		}

		if (_attackPlayer->GetMaxHp() != _defencePlayer->GetMaxHp())
		{
			return false;
		}
			
		if (_attackPlayer->IsHPZero() == true ||
			_defencePlayer->IsHPZero() == true)
		{
			return false;
		}


		return true;

	}

	bool Match::Bomb(Player * thisPlayer, int EnemyTileIndex)
	{
		if (_attackPlayer != thisPlayer)
		{
			return false;
		}

		if (_defencePlayer->GetMaxHp() >= EnemyTileIndex)
		{
			return false;
		}
		int tileInfo = _defencePlayer->GetTileInfo(EnemyTileIndex);

		if (tileInfo < 0)
		{
			return false;
		}
		else if (tileInfo == 0)
		{
			changeTurn();
			return false;
		}
		else
		{
			_defencePlayer->Hitted(EnemyTileIndex);

			return true;
		}
	}

	bool Match::IsEndGame()
	{
		if (_defencePlayer->GetHp() <= 0)
		{
			return true;
		}

		return false;
	}

	unsigned long long Match::getRandomNumber()
	{
		auto curTime = std::chrono::system_clock::now();
		auto duration = curTime.time_since_epoch();
		auto milliSec = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		std::mt19937 mtRand(milliSec);

		return mtRand();
	}

}
