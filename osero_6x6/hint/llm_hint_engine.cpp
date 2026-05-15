#include "hint/llm_hint_engine.hpp"

#include <stdexcept>

namespace reversi
{

LlmHintEngine::LlmHintEngine(std::shared_ptr<LlmClient> llm_client, PromptBuilder prompt_builder)
    : m_llm_client(llm_client), m_prompt_builder(prompt_builder)
{
	if (!m_llm_client) {
		throw std::runtime_error("LLM client is not configured.");
	}
}

HintResult LlmHintEngine::getHint(const Board& board, Side side)
{
	HintResult result = m_local_hint_engine.getHint(board, side);
	const std::string prompt = m_prompt_builder.build(board, side, result);

	result.explanation = m_llm_client->generateHint(prompt);
	return result;
}

}  // namespace reversi
