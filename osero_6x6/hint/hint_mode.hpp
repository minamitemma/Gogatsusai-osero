#pragma once

namespace reversi
{

enum class HintMode
{
	NONE,
	LOCAL,
	LLM
};

enum class HintFormat
{
	TEXT,
	JSON
};

}  // namespace reversi
