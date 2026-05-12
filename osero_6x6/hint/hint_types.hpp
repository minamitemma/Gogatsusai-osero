#pragma once

#include <string>
#include <vector>

#include "board.hpp"

namespace reversi
{

struct HintCandidate
{
	CellPosition move;
	int score;
	std::string reason;
};

struct HintResult
{
	CellPosition best_move;
	int evaluation_score;
	int search_depth = 0;
	std::string position_summary;
	std::string explanation;
	std::vector<HintCandidate> candidates;
};

}  // namespace reversi
