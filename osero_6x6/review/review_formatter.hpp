#pragma once

#include <string>

#include "review/game_record.hpp"

namespace reversi
{

std::string formatReviewJson(const GameRecord& record, const std::string& summary);

}  // namespace reversi
