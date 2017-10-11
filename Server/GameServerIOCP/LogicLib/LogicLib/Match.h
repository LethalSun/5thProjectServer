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

		void InitIndex(int idx) { _indexInPool = idx; }

		int GetIndex() { return _indexInPool; }

		bool SetPlayers(Player* player1, Player* player2);

		bool Reset();


		bool IsMatchCorrect();

		bool IsMyTurn(Player* player)
		{
			if (_attackPlayer == player)
			{
				return true;
			}
			else if(_defencePlayer == player)
			{
				return false;
			}
			else
			{
				return false;
			}
		}

		bool Bomb(Player* thisPlayer, int EnemyTileIndex);

		bool IsEndGame();

		Player* GetEnemy(Player* me)
		{
			if (_attackPlayer == me)
			{
				return _defencePlayer;
			}
			else if (_defencePlayer == me)
			{
				return _attackPlayer;
			}
			else
			{
				return nullptr;
			}
		}
	private:

		unsigned long long getRandomNumber();

		bool changeTurn();

	private:
		int _indexInPool = 0;
		bool _isUsed = false;

		Player* _attackPlayer = nullptr;
		Player* _defencePlayer = nullptr;

	};

}


