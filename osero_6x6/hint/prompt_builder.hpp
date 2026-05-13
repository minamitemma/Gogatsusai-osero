#pragma once

#include <string>

#include "board.hpp"
#include "hint/hint_types.hpp"

namespace reversi
{

class PromptBuilder
{
public:
	std::string build(const Board& board, Side side, const HintResult& local_hint) const;
};

}  // namespace reversi
