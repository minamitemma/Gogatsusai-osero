#include "hint/llm_hint_engine.hpp"

namespace reversi
{

LlmHintEngine::LlmHintEngine(GeminiClient gemini_client, PromptBuilder prompt_builder)
    : m_gemini_client(gemini_client), m_prompt_builder(prompt_builder)
{
}

HintResult LlmHintEngine::getHint(const Board& board, Side side)
{
	HintResult result = m_local_hint_engine.getHint(board, side);
	const std::string prompt = m_prompt_builder.build(board, side, result);

	result.explanation = m_gemini_client.generateHint(prompt);
	return result;
}

}  // namespace reversi
