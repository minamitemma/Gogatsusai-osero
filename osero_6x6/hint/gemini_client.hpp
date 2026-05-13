#pragma once

#include <string>

namespace reversi
{

class GeminiClient
{
public:
	GeminiClient(std::string api_key, std::string model_name = "gemini-3-flash-preview");

	std::string generateHint(const std::string& prompt) const;

private:
	std::string m_api_key;
	std::string m_model_name;
};

}  // namespace reversi
