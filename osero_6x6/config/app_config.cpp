#include "config/app_config.hpp"

#include <cstdlib>
#include <stdexcept>
#include <string>

namespace reversi
{

std::string getGeminiApiKey()
{
	const char* value = std::getenv("GEMINI_API_KEY");
	if (value == nullptr || std::string(value).empty()) {
		throw std::runtime_error("GEMINI_API_KEY is not set.");
	}
	return value;
}

std::string getGeminiModelName()
{
	const char* value = std::getenv("GEMINI_MODEL");
	if (value == nullptr || std::string(value).empty()) {
		return "gemini-3-flash-preview";
	}
	return value;
}

std::string getOpenAiApiKey()
{
	const char* value = std::getenv("OPENAI_API_KEY");
	if (value == nullptr || std::string(value).empty()) {
		throw std::runtime_error("OPENAI_API_KEY is not set.");
	}
	return value;
}

std::string getOpenAiModelName()
{
	const char* value = std::getenv("OPENAI_MODEL");
	if (value == nullptr || std::string(value).empty()) {
		return "gpt-5.4-mini";
	}
	return value;
}

std::string getLlmProvider()
{
	const char* value = std::getenv("LLM_PROVIDER");
	if (value == nullptr || std::string(value).empty()) {
		return "gemini";
	}
	return value;
}

}  // namespace reversi
