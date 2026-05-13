#pragma once

#include <vector>

#include "board.hpp"

namespace reversi
{

struct MoveRecord
{
	int turn_number;
	Side side;
	bool was_pass;
	CellPosition move;
	int black_eval_before;
	int black_eval_after;
	int delta_for_mover;
	int black_count_after;
	int white_count_after;
};

struct GameRecord
{
	std::vector<MoveRecord> moves;
	bool has_human_side = false;
	Side human_side = Side::BLACK;
	int final_black_count = 0;
	int final_white_count = 0;
};

}  // namespace reversi
