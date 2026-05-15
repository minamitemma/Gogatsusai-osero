#pragma once

#include <string>

namespace reversi
{

class LlmClient
{
public:
	virtual ~LlmClient() = default;

	virtual std::string generateHint(const std::string& prompt) const = 0;
};

}  // namespace reversi
