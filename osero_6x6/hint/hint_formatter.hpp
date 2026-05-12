#pragma once

#include <string>

#include "board.hpp"
#include "hint/hint_types.hpp"

namespace reversi
{

std::string formatHintText(const HintResult& hint);
std::string formatHintJson(const HintResult& hint, Side side);

}  // namespace reversi
