#pragma once

#include <string>

#include "review/game_record.hpp"

namespace reversi
{

class ReviewPromptBuilder
{
public:
	std::string build(const GameRecord& record) const;
};

}  // namespace reversi
