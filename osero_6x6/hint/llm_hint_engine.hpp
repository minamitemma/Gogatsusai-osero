#pragma once

#include "hint/gemini_client.hpp"
#include "hint/hint_engine.hpp"
#include "hint/local_hint_engine.hpp"
#include "hint/prompt_builder.hpp"

namespace reversi
{

class LlmHintEngine : public HintEngine
{
public:
	LlmHintEngine(GeminiClient gemini_client, PromptBuilder prompt_builder = PromptBuilder());

	virtual HintResult getHint(const Board& board, Side side) override;

private:
	GeminiClient m_gemini_client;
	PromptBuilder m_prompt_builder;
	LocalHintEngine m_local_hint_engine;
};

}  // namespace reversi
