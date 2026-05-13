#pragma once

#include <chrono>

#include "board.hpp"
#include "player.hpp"

namespace reversi
{

class MinmaxPlayer : public Player
{
public:
	// time_limit: AIの1手あたりの制限時間
	MinmaxPlayer(Side side, std::chrono::milliseconds time_limit = std::chrono::seconds(3))
	    : Player(side), m_time_limit(time_limit)
	{
	}

	virtual CellPosition thinkNextMove(const Board& board) override;

private:
	std::chrono::milliseconds m_time_limit;
};

}  // namespace reversi
