#pragma once

#include "board.hpp"
#include "player.hpp"

namespace reversi
{

class HumanPlayer : public Player
{
public:
	HumanPlayer(Side side) : Player(side) {}

	virtual CellPosition thinkNextMove(const Board& board) override;
};

}  // namespace reversi
