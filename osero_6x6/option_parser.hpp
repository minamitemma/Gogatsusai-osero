#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "hint/hint_mode.hpp"
#include "player.hpp"
#include "review/review_mode.hpp"

namespace reversi
{

struct CommandLineParams
{
	std::unique_ptr<Player> black_player;
	std::unique_ptr<Player> white_player;
	std::string black_player_type;
	std::string white_player_type;
	HintMode hint_mode = HintMode::NONE;
	HintFormat hint_format = HintFormat::TEXT;
	ReviewMode review_mode = ReviewMode::NONE;
	HintFormat review_format = HintFormat::TEXT;
};

using PlayerTypeMap = std::unordered_map<std::string, std::function<std::unique_ptr<Player>(Side)>>;

CommandLineParams parseCommandLineOptions(int argc, char* argv[], const PlayerTypeMap& player_types);

}  // namespace reversi
