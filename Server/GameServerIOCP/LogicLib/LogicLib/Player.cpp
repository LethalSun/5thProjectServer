#include "pch.h"
#include "Player.h"
#include "Match.h"
namespace LogicLib
{
	Player::~Player()
	{
	}
	bool Player::IsPlaying()
	{
		if (_state != PlayerState::PLAYING)
		{
			return false;
		}

		if (_matchInfo == nullptr)
		{
			return false;
		}

		if (_matchInfo->IsMatchCorrect() != true)
		{
			return false;
		}

		return true;
	}
	bool Player::Bomb(int idx)
	{
		if (IsMyTurn() == false)
		{
			return false;
		}

		if (_matchInfo->Bomb(this, idx) == false)
		{
			return false;
		}

		return true;

	}
	bool Player::IsMyTurn()
	{
		if (IsPlaying() == false)
		{
			return false;
		}

		if (_matchInfo->IsMyTurn(this) == false)
		{
			return false;
		}

		return true;
	}
	Player * Player::GetEnemy()
	{
		if (_matchInfo == nullptr)
		{
			return nullptr;
		}

		return _matchInfo->GetEnemy(this);
	}
}
