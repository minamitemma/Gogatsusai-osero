#pragma once

#include <chrono>
#include <vector>

#include "board.hpp"
#include "hint/hint_types.hpp"

namespace reversi
{

struct MinmaxHintEvaluation
{
	std::vector<HintCandidate> candidates;
	int completed_depth = 0;
};

MinmaxHintEvaluation evaluateHintCandidatesWithMinmax(
    const Board& board,
    Side side,
    std::chrono::milliseconds time_limit = std::chrono::seconds(3));

}  // namespace reversi
