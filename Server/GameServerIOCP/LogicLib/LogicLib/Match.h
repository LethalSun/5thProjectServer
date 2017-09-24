#pragma once
namespace LogicLib
{
	class Player;

	class Match
	{
	public:
		Match()
		{

		}

		~Match();

		bool SetPlayers(Player* player1, Player* player2);

		bool Bomb(Player* player, int tileInfo);

		bool EndGame();
	private:

		unsigned long long getRandomNumber();
		
		bool isBombCorrect(Player* player, int tileInfo);

		bool changeTurn();

	private:

		int _MatchID = -1;

		Player* _attackPlayer = nullptr;
		Player* _defencePlayer = nullptr;

	};

}


