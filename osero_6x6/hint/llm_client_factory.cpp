#include "hint/llm_client_factory.hpp"

#include <memory>
#include <stdexcept>
#include <string>

#include "config/app_config.hpp"
#include "hint/gemini_client.hpp"
#include "hint/openai_client.hpp"

namespace reversi
{

std::shared_ptr<LlmClient> makeConfiguredLlmClient()
{
	const std::string provider = getLlmProvider();
	if (provider == "gemini") {
		return std::make_shared<GeminiClient>(getGeminiApiKey(), getGeminiModelName());
	}
	if (provider == "openai") {
		return std::make_shared<OpenAiClient>(getOpenAiApiKey(), getOpenAiModelName());
	}
	throw std::runtime_error("Unsupported LLM_PROVIDER: " + provider);
}

std::string configuredLlmProviderLabel()
{
	const std::string provider = getLlmProvider();
	if (provider == "openai") {
		return "OPENAI HINT";
	}
	return "GEMINI HINT";
}

}  // namespace reversi
