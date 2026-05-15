#pragma once

#include "hint/hint_engine.hpp"
#include "hint/llm_client.hpp"
#include "hint/local_hint_engine.hpp"
#include "hint/prompt_builder.hpp"

#include <memory>

namespace reversi
{

class LlmHintEngine : public HintEngine
{
public:
	LlmHintEngine(std::shared_ptr<LlmClient> llm_client, PromptBuilder prompt_builder = PromptBuilder());

	virtual HintResult getHint(const Board& board, Side side) override;

private:
	std::shared_ptr<LlmClient> m_llm_client;
	PromptBuilder m_prompt_builder;
	LocalHintEngine m_local_hint_engine;
};

}  // namespace reversi
