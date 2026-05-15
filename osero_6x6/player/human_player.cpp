#include <iostream>
#include <future>
#include <stdexcept>
#include <string>
#include <thread>

#include "config/app_config.hpp"
#include "hint/hint_formatter.hpp"
#include "hint/llm_client_factory.hpp"
#include "hint/llm_hint_engine.hpp"
#include "hint/local_hint_engine.hpp"
#include "human_player.hpp"

namespace reversi
{

namespace
{

void printHintResult(const HintResult& hint, Side side, HintFormat hint_format, const std::string& label)
{
	std::cout << "\n[" << label << "]\n";
	if (hint_format == HintFormat::JSON) {
		std::cout << formatHintJson(hint, side) << "\n";
	} else {
		std::cout << formatHintText(hint);
	}
	std::cout << std::endl;
}

void printLocalHint(const Board& board, Side side, HintFormat hint_format)
{
	LocalHintEngine hint_engine;
	const HintResult hint = hint_engine.getHint(board, side);

	printHintResult(hint, side, hint_format, "hint");
}

void printLlmHint(const Board& board, Side side, HintFormat hint_format)
{
	auto future_hint = std::async(std::launch::async, [&board, side]() {
		LlmHintEngine hint_engine(makeConfiguredLlmClient());
		return hint_engine.getHint(board, side);
	});

	int dots = 1;
	while (future_hint.wait_for(std::chrono::milliseconds(250)) != std::future_status::ready) {
		std::cout << "\rちょっと待ってね";
		for (int i = 0; i < dots; ++i) {
			std::cout << "・";
		}
		std::cout << "      " << std::flush;
		dots = dots % 6 + 1;
	}
	std::cout << "\r" << std::string(40, ' ') << "\r" << std::flush;

	const HintResult hint = future_hint.get();

	printHintResult(hint, side, hint_format, "llm hint");
}

}  // namespace

void HumanPlayer::configureHints(HintMode hint_mode, HintFormat hint_format, int hint_count)
{
	m_hint_mode = hint_mode;
	m_hint_format = hint_format;
	m_remaining_hints = hint_count;
}

CellPosition HumanPlayer::thinkNextMove(const Board& board)
{
	while (true) {
		std::cout << "Where?";
		if (m_hint_mode != HintMode::NONE) {
			std::cout << " (hint: h, remaining: " << m_remaining_hints << ")";
		}
		std::cout << " ";

		std::string position_str;
		std::cin >> position_str;
		if (!std::cin) {
			std::exit(0);
		}

		if (position_str == "h") {
			if (m_hint_mode == HintMode::NONE) {
				std::cout << "Hint is disabled.\n" << std::endl;
				continue;
			}
			if (m_remaining_hints <= 0) {
				std::cout << "No hints remaining.\n" << std::endl;
				continue;
			}
			try {
				if (m_hint_mode == HintMode::LLM) {
					printLlmHint(board, getSide(), m_hint_format);
				} else {
					printLocalHint(board, getSide(), m_hint_format);
				}
			} catch (const std::exception& ex) {
				std::cout << "Hint failed: " << ex.what() << "\n" << std::endl;
				continue;
			}

			--m_remaining_hints;
			continue;
		}

		try {
			auto move = parseCellPosition(position_str);
			if (board.isLegalMove(move, getSide())) {
				return move;
			}
			std::cout << position_str << " is not a legal move!\n"
			          << std::endl;
		} catch (const std::invalid_argument& ex) {
			std::cout << ex.what() << "\n"
			          << std::endl;
			continue;
		}
	}
}

}  // namespace reversi
