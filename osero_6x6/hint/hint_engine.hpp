#pragma once

#include "board.hpp"
#include "hint/hint_types.hpp"

namespace reversi
{

class HintEngine
{
public:
	virtual ~HintEngine() = default;

	virtual HintResult getHint(const Board& board, Side side) = 0;
};

}  // namespace reversi
