#pragma once

#include "board.hpp"
#include "hint/hint_mode.hpp"
#include "player.hpp"

namespace reversi
{

class HumanPlayer : public Player
{
public:
	HumanPlayer(Side side) : Player(side) {}

	void configureHints(HintMode hint_mode, HintFormat hint_format, int hint_count);

	virtual CellPosition thinkNextMove(const Board& board) override;

private:
	HintMode m_hint_mode = HintMode::NONE;
	HintFormat m_hint_format = HintFormat::TEXT;
	int m_remaining_hints = 0;
};

}  // namespace reversi
