#include <array>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "board.hpp"
#include "option_parser.hpp"
#include "player.hpp"
#include "player/human_player.hpp"
#include "player/minmax_player.hpp"
#include "player/sample_computer_player.hpp"

int main(int argc, char* argv[])
{
	using namespace reversi;

	const std::unordered_map<std::string, std::function<std::unique_ptr<Player>(Side)>> player_types = {
	    {"human", [](Side side) { return std::unique_ptr<Player>(new HumanPlayer(side)); }},
	    {"sample", [](Side side) { return std::unique_ptr<Player>(new SampleComputerPlayer(side)); }},
	    {"minmax", [](Side side) { return std::unique_ptr<Player>(new MinmaxPlayer(side)); }},
	};

	auto command_line_params = parseCommandLineOptions(argc, argv, player_types);

	std::array<std::unique_ptr<Player>, 2> players;
	players.at(static_cast<std::size_t>(Side::BLACK)) = std::move(command_line_params.black_player);
	players.at(static_cast<std::size_t>(Side::WHITE)) = std::move(command_line_params.white_player);

	Board board;

	int pass = 0;  // 相互パスの判定

	for (Side turn = Side::BLACK;; turn = getOpponentSide(turn)) {
		std::cout << board << "\n"
		          << std::endl;

		if (board.count(CellState::EMPTY) == 0) {
			// 盤面が埋まったので終了
			break;
		}
		auto legal_moves = board.getAllLegalMoves(turn);
		if (legal_moves.empty()) {
			pass++;
			if (pass >= 2) {
				std::printf("両者パスしたのでこの試合は終了です。\n");
				break;
			}
			std::cout << "turn = " << turn << ", move = Pass\n"
			          << std::endl;
			continue;
		}

		auto& turn_player = *players.at(static_cast<std::size_t>(turn));

		auto move = turn_player.thinkNextMove(board);
		if (!board.isLegalMove(move, turn)) {
			std::cout << "turn = " << turn << ", illegal move = " << move << std::endl;
			std::exit(1);
		}
		std::cout << "turn = " << turn << ", move = " << move << "\n"
		          << std::endl;

		board.placeDisk(move, turn);
		pass = 0;  // 2連パス以外は通さない
	}

	// 勝敗の判定
	int count_w = 0;
	int count_b = 0;
	for (int y = 0; y < Board::HEIGHT; ++y) {
		for (int x = 0; x < Board::WIDTH; ++x) {
			CellState state = board.get({x, y});
			switch (state) {
			case CellState::EMPTY:
				break;
			case CellState::BLACK:
				count_b++;
				break;
			case CellState::WHITE:
				count_w++;
				break;
			}
		}
	}
	std::printf("白が %d 個、黒が %d 個なので", count_w, count_b);
	if (count_b > count_w) {
		std::printf("黒の勝ちです。");
	} else if (count_b < count_w) {
		std::printf("白の勝ちです。");
	} else {
		std::printf("引き分けです。");
	}
	std::printf("\n");

	return 0;
}
