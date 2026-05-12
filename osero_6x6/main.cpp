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
#include "evaluator.hpp"
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

	auto signed_str = [](int v) {
		return (v > 0 ? std::string("+") : std::string("")) + std::to_string(v);
	};

	auto printPosition = [&](const Board& b) {
		int black_eval = evaluate(b, Side::BLACK);
		std::cout << "形勢: 黒視点 " << signed_str(black_eval)
		          << " / 白視点 " << signed_str(-black_eval)
		          << "  [盤上: 黒 " << b.count(CellState::BLACK)
		          << ", 白 " << b.count(CellState::WHITE) << "]"
		          << std::endl;
	};

	bool has_prev_move = false;
	Side prev_mover = Side::BLACK;       // 初期値はダミー
	int prev_black_eval_before_move = 0; // 直前の手を打つ直前の (黒視点) 評価値

	for (Side turn = Side::BLACK;; turn = getOpponentSide(turn)) {
		std::cout << board << "\n"
		          << std::endl;

		// 直前の手で どれだけ動いたか
		if (has_prev_move) {
			int cur_black_eval = evaluate(board, Side::BLACK);
			int delta_black = cur_black_eval - prev_black_eval_before_move;
			int delta_for_mover = (prev_mover == Side::BLACK) ? delta_black : -delta_black;
			std::cout << "└ 直前手 (" << prev_mover << ") の評価変化: "
			          << prev_mover << "側 " << signed_str(delta_for_mover) << std::endl;
		}
		printPosition(board);
		std::cout << std::endl;

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
			has_prev_move = false;  // パスは差分の起点にしない
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

		// 着手直前の状態を記録 → 次ループ冒頭で差分表示
		prev_black_eval_before_move = evaluate(board, Side::BLACK);
		prev_mover = turn;
		has_prev_move = true;

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
