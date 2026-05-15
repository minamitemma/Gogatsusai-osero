#pragma once

#include <string>

#include "hint/llm_client.hpp"

namespace reversi
{

class OpenAiClient : public LlmClient
{
public:
	OpenAiClient(std::string api_key, std::string model_name = "gpt-5.4-mini");

	virtual std::string generateHint(const std::string& prompt) const override;

private:
	std::string m_api_key;
	std::string m_model_name;
};

}  // namespace reversi
