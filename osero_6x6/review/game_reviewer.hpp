#pragma once

#include <string>

#include "hint/llm_client.hpp"
#include "review/game_record.hpp"
#include "review/review_prompt_builder.hpp"

namespace reversi
{

class GameReviewer
{
public:
	GameReviewer(ReviewPromptBuilder prompt_builder = ReviewPromptBuilder());

	std::string buildLocalReview(const GameRecord& record) const;
	std::string buildLlmReview(const GameRecord& record, const LlmClient& llm_client) const;

private:
	ReviewPromptBuilder m_prompt_builder;
};

}  // namespace reversi
