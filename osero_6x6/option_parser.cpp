#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "board.hpp"
#include "option_parser.hpp"
#include "player.hpp"

namespace reversi
{

namespace
{

bool findHelpOption(int argc, char* argv[])
{
	using namespace std::literals::string_literals;
	for (int i = 1; i < argc; ++i) {
		if (argv[i] == "-h"s || argv[i] == "--help"s) {
			return true;
		}
	}
	return false;
}

void printSimpleUsage(std::ostream& os, const char* program_name)
{
	os << "usage: " << program_name
	   << " [-h] [--hint none|local|llm] [--hint-format text|json] [--review none|local|llm] [--review-format text|json] BLACK_PLAYER_TYPE WHITE_PLAYER_TYPE"
	   << std::endl;
}

std::unique_ptr<Player> makePlayerFromType(const PlayerTypeMap& player_types, const std::string& type, const Side side)
{
	auto it = player_types.find(type);
	if (it == player_types.end()) {
		std::cerr << "\'" << type << "\' is not a valid player type!" << std::endl;
		std::exit(1);
	}
	return it->second(side);
}

HintMode parseHintMode(const std::string& value)
{
	if (value == "none") {
		return HintMode::NONE;
	}
	if (value == "local") {
		return HintMode::LOCAL;
	}
	if (value == "llm") {
		return HintMode::LLM;
	}

	std::cerr << "\'" << value << "\' is not a valid hint mode!" << std::endl;
	std::cerr << "Choose from: none local llm" << std::endl;
	std::exit(1);
}

HintFormat parseHintFormat(const std::string& value)
{
	if (value == "text") {
		return HintFormat::TEXT;
	}
	if (value == "json") {
		return HintFormat::JSON;
	}

	std::cerr << "\'" << value << "\' is not a valid hint format!" << std::endl;
	std::cerr << "Choose from: text json" << std::endl;
	std::exit(1);
}

ReviewMode parseReviewMode(const std::string& value)
{
	if (value == "none") {
		return ReviewMode::NONE;
	}
	if (value == "local") {
		return ReviewMode::LOCAL;
	}
	if (value == "llm") {
		return ReviewMode::LLM;
	}

	std::cerr << "\'" << value << "\' is not a valid review mode!" << std::endl;
	std::cerr << "Choose from: none local llm" << std::endl;
	std::exit(1);
}

}  // namespace

CommandLineParams parseCommandLineOptions(int argc, char* argv[], const PlayerTypeMap& player_types)
{
	if (findHelpOption(argc, argv)) {
		printSimpleUsage(std::cout, argv[0]);
		std::cout << "\n"
		             "  [BLACK/WHITE]_PLAYER_TYPE      Player Style.\n"
		             "                                 Choose from:";
		for (auto&& type : player_types) {
			std::cout << " " << type.first;
		}
		std::cout << "\n"
		             "  --hint none|local|llm          Hint mode. Default: none.\n"
		             "  --hint-format text|json        Hint output format. Default: text.\n"
		             "  --review none|local|llm        Post-game review mode. Default: none.\n"
		             "  --review-format text|json      Review output format. Default: text.\n";
		std::exit(1);
	}

	HintMode hint_mode = HintMode::NONE;
	HintFormat hint_format = HintFormat::TEXT;
	ReviewMode review_mode = ReviewMode::NONE;
	HintFormat review_format = HintFormat::TEXT;
	std::vector<std::string> positional_args;

	for (int i = 1; i < argc; ++i) {
		const std::string arg = argv[i];
		if (arg == "--hint") {
			if (i + 1 >= argc) {
				std::cerr << "--hint requires an argument!" << std::endl;
				printSimpleUsage(std::cerr, argv[0]);
				std::cerr << "Try '" << argv[0] << " -h' for more information." << std::endl;
				std::exit(1);
			}
			hint_mode = parseHintMode(argv[++i]);
		} else if (arg.compare(0, 7, "--hint=") == 0) {
			hint_mode = parseHintMode(arg.substr(7));
		} else if (arg == "--hint-format") {
			if (i + 1 >= argc) {
				std::cerr << "--hint-format requires an argument!" << std::endl;
				printSimpleUsage(std::cerr, argv[0]);
				std::cerr << "Try '" << argv[0] << " -h' for more information." << std::endl;
				std::exit(1);
			}
			hint_format = parseHintFormat(argv[++i]);
		} else if (arg.compare(0, 14, "--hint-format=") == 0) {
			hint_format = parseHintFormat(arg.substr(14));
		} else if (arg == "--review") {
			if (i + 1 >= argc) {
				std::cerr << "--review requires an argument!" << std::endl;
				printSimpleUsage(std::cerr, argv[0]);
				std::cerr << "Try '" << argv[0] << " -h' for more information." << std::endl;
				std::exit(1);
			}
			review_mode = parseReviewMode(argv[++i]);
		} else if (arg.compare(0, 9, "--review=") == 0) {
			review_mode = parseReviewMode(arg.substr(9));
		} else if (arg == "--review-format") {
			if (i + 1 >= argc) {
				std::cerr << "--review-format requires an argument!" << std::endl;
				printSimpleUsage(std::cerr, argv[0]);
				std::cerr << "Try '" << argv[0] << " -h' for more information." << std::endl;
				std::exit(1);
			}
			review_format = parseHintFormat(argv[++i]);
		} else if (arg.compare(0, 16, "--review-format=") == 0) {
			review_format = parseHintFormat(arg.substr(16));
		} else if (!arg.empty() && arg[0] == '-') {
			std::cerr << "Unknown option: " << arg << std::endl;
			printSimpleUsage(std::cerr, argv[0]);
			std::cerr << "Try '" << argv[0] << " -h' for more information." << std::endl;
			std::exit(1);
		} else {
			positional_args.push_back(arg);
		}
	}

	if (positional_args.size() < 2) {
		std::cerr << "Too few arguments!" << std::endl;
		printSimpleUsage(std::cerr, argv[0]);
		std::cerr << "Try '" << argv[0] << " -h' for more information." << std::endl;
		std::exit(1);
	}
	if (positional_args.size() > 2) {
		std::cerr << "Too many arguments!" << std::endl;
		printSimpleUsage(std::cerr, argv[0]);
		std::cerr << "Try '" << argv[0] << " -h' for more information." << std::endl;
		std::exit(1);
	}
	CommandLineParams params;
	params.black_player_type = positional_args.at(0);
	params.white_player_type = positional_args.at(1);
	params.black_player = makePlayerFromType(player_types, positional_args.at(0), Side::BLACK);
	params.white_player = makePlayerFromType(player_types, positional_args.at(1), Side::WHITE);
	params.hint_mode = hint_mode;
	params.hint_format = hint_format;
	params.review_mode = review_mode;
	params.review_format = review_format;
	return params;
}

}  // namespace reversi
