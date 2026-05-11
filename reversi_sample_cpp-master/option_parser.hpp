#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "player.hpp"

namespace reversi
{

struct CommandLineParams
{
	std::unique_ptr<Player> black_player;
	std::unique_ptr<Player> white_player;
};

using PlayerTypeMap = std::unordered_map<std::string, std::function<std::unique_ptr<Player>(Side)>>;

CommandLineParams parseCommandLineOptions(int argc, char* argv[], const PlayerTypeMap& player_types);

}  // namespace reversi
