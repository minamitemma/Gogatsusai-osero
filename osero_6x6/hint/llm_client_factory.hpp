#pragma once

#include <memory>

#include "hint/llm_client.hpp"

namespace reversi
{

std::shared_ptr<LlmClient> makeConfiguredLlmClient();
std::string configuredLlmProviderLabel();

}  // namespace reversi
