#pragma once

#include <tuple>

#include "board.hpp"

namespace reversi
{

class Player
{
public:
	Player(Side side) : m_side(side) {}
	virtual ~Player() = default;

	Side getSide() const
	{
		return m_side;
	}

	virtual CellPosition thinkNextMove(const Board& board) = 0;

private:
	Side m_side;
};

}  // namespace reversi
