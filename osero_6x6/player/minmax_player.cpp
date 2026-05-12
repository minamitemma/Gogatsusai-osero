#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "evaluator.hpp"
#include "minmax_player.hpp"

namespace reversi
{

namespace
{

// 勝敗確定時の評価値(中盤評価より十分大きく取る)
constexpr int WIN_SCORE = 1000000;

// 時間切れ用例外
class TimeoutException : public std::exception
{
};

using Clock = std::chrono::steady_clock;

// 手の並べ替え用: 位置重みが大きい順に試す(α-β枝刈り効率化)
void orderMoves(std::vector<CellPosition>& moves, CellPosition prev_best)
{
	if (Board::boundsCheck(prev_best)) {
		auto it = std::find_if(moves.begin(), moves.end(),
		                       [&](CellPosition p) { return p.x == prev_best.x && p.y == prev_best.y; });
		if (it != moves.end()) {
			std::iter_swap(moves.begin(), it);
		}
	}
	std::stable_sort(moves.begin() + (Board::boundsCheck(prev_best) ? 1 : 0), moves.end(),
	                 [](CellPosition a, CellPosition b) {
		                 return positionWeight(a.x, a.y) > positionWeight(b.x, b.y);
	                 });
}

// α-β探索 (negamax 形式ではなく、ai_side 固定の min-max)
// 戻り値は ai_side から見た評価値
int alphabeta(Board board, Side turn, Side ai_side, int depth, int alpha, int beta,
              const Clock::time_point& deadline, bool prev_passed)
{
	if (Clock::now() >= deadline) {
		throw TimeoutException();
	}

	auto legal_moves = board.getAllLegalMoves(turn);
	if (legal_moves.empty()) {
		if (prev_passed) {
			// 両者パス → 終局
			return evaluate(board, ai_side);
		}
		// パス: 手番を渡してそのまま続行(深さは消費しない)
		return alphabeta(board, getOpponentSide(turn), ai_side, depth, alpha, beta, deadline, true);
	}

	if (depth == 0) {
		return evaluate(board, ai_side);
	}

	orderMoves(legal_moves, {-1, -1});

	const bool maximizing = (turn == ai_side);
	if (maximizing) {
		int best = std::numeric_limits<int>::min() / 2;
		for (auto move : legal_moves) {
			Board next = board;
			next.placeDisk(move, turn);
			int score = alphabeta(next, getOpponentSide(turn), ai_side, depth - 1, alpha, beta, deadline, false);
			if (score > best) best = score;
			if (best > alpha) alpha = best;
			if (alpha >= beta) break;  // β cut
		}
		return best;
	} else {
		int best = std::numeric_limits<int>::max() / 2;
		for (auto move : legal_moves) {
			Board next = board;
			next.placeDisk(move, turn);
			int score = alphabeta(next, getOpponentSide(turn), ai_side, depth - 1, alpha, beta, deadline, false);
			if (score < best) best = score;
			if (best < beta) beta = best;
			if (alpha >= beta) break;  // α cut
		}
		return best;
	}
}

}  // namespace

CellPosition MinmaxPlayer::thinkNextMove(const Board& board)
{
	const Side ai_side = getSide();
	auto legal_moves = board.getAllLegalMoves(ai_side);
	// パスは main 側で処理されるのでここには来ないはず
	if (legal_moves.empty()) {
		return {-1, -1};
	}
	if (legal_moves.size() == 1) {
		std::cout << "(minmax: 唯一の合法手)" << std::endl;
		return legal_moves.front();
	}

	const auto deadline = Clock::now() + m_time_limit;
	// 残り空きマスを越えて深く探索しても意味がないので上限にする
	const int max_depth = board.count(CellState::EMPTY);

	CellPosition best_move = legal_moves.front();
	int best_completed_depth = 0;
	int best_completed_score = 0;

	// 反復深化: depth = 1, 2, 3, ...
	for (int depth = 1; depth <= max_depth; ++depth) {
		CellPosition iter_best = legal_moves.front();
		int iter_best_score = std::numeric_limits<int>::min() / 2;
		int alpha = std::numeric_limits<int>::min() / 2;
		const int beta = std::numeric_limits<int>::max() / 2;

		auto moves_ordered = legal_moves;
		orderMoves(moves_ordered, best_move);

		bool completed = true;
		try {
			for (auto move : moves_ordered) {
				Board next = board;
				next.placeDisk(move, ai_side);
				int score = alphabeta(next, getOpponentSide(ai_side), ai_side,
				                      depth - 1, alpha, beta, deadline, false);
				if (score > iter_best_score) {
					iter_best_score = score;
					iter_best = move;
				}
				if (iter_best_score > alpha) alpha = iter_best_score;
			}
		} catch (const TimeoutException&) {
			completed = false;
		}

		if (completed) {
			best_move = iter_best;
			best_completed_depth = depth;
			best_completed_score = iter_best_score;
			// 勝敗が見えたら早期打ち切り
			if (iter_best_score >= WIN_SCORE || iter_best_score <= -WIN_SCORE) {
				break;
			}
		} else {
			// 時間切れ: 直前に完了した深さの結果を採用
			break;
		}

		if (Clock::now() >= deadline) break;
	}

	std::cout << "(minmax: depth=" << best_completed_depth
	          << ", score=" << best_completed_score << ")" << std::endl;
	return best_move;
}

}  // namespace reversi
