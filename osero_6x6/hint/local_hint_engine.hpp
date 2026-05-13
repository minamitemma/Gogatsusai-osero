#pragma once

#include "hint/hint_engine.hpp"

namespace reversi
{

class LocalHintEngine : public HintEngine
{
public:
	virtual HintResult getHint(const Board& board, Side side) override;
};

}  // namespace reversi
