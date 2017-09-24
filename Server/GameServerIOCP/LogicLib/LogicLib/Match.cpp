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

		return true;
	}

	bool Match::changeTurn()
	{
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
