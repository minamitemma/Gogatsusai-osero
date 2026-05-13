#include "hint/minmax_hint_evaluator.hpp"

#include <algorithm>
#include <chrono>
#include <limits>
#include <stdexcept>
#include <vector>

#include "evaluator.hpp"

namespace reversi
{

namespace
{

constexpr int WIN_SCORE = 1000000;

class TimeoutException : public std::exception
{
};

using Clock = std::chrono::steady_clock;

bool samePosition(CellPosition lhs, CellPosition rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

void orderMoves(std::vector<CellPosition>& moves, CellPosition prev_best)
{
	const bool has_prev_best = Board::boundsCheck(prev_best);
	if (has_prev_best) {
		auto it = std::find_if(moves.begin(), moves.end(),
		                       [&](CellPosition p) { return samePosition(p, prev_best); });
		if (it != moves.end()) {
			std::iter_swap(moves.begin(), it);
		}
	}

	std::stable_sort(moves.begin() + (has_prev_best ? 1 : 0), moves.end(),
	                 [](CellPosition a, CellPosition b) {
		                 return positionWeight(a.x, a.y) > positionWeight(b.x, b.y);
	                 });
}

int alphabeta(Board board, Side turn, Side ai_side, int depth, int alpha, int beta,
              const Clock::time_point& deadline, bool prev_passed)
{
	if (Clock::now() >= deadline) {
		throw TimeoutException();
	}

	auto legal_moves = board.getAllLegalMoves(turn);
	if (legal_moves.empty()) {
		if (prev_passed) {
			return evaluate(board, ai_side);
		}
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
			const int score = alphabeta(next, getOpponentSide(turn), ai_side, depth - 1, alpha, beta, deadline, false);
			if (score > best) {
				best = score;
			}
			if (best > alpha) {
				alpha = best;
			}
			if (alpha >= beta) {
				break;
			}
		}
		return best;
	}

	int best = std::numeric_limits<int>::max() / 2;
	for (auto move : legal_moves) {
		Board next = board;
		next.placeDisk(move, turn);
		const int score = alphabeta(next, getOpponentSide(turn), ai_side, depth - 1, alpha, beta, deadline, false);
		if (score < best) {
			best = score;
		}
		if (best < beta) {
			beta = best;
		}
		if (alpha >= beta) {
			break;
		}
	}
	return best;
}

std::vector<HintCandidate> evaluateOnePlyFallback(const Board& board, Side side, const std::vector<CellPosition>& legal_moves)
{
	std::vector<HintCandidate> candidates;
	for (auto move : legal_moves) {
		Board next = board;
		next.placeDisk(move, side);
		candidates.push_back({move, evaluate(next, side), ""});
	}
	return candidates;
}

void sortCandidates(std::vector<HintCandidate>& candidates)
{
	std::stable_sort(candidates.begin(), candidates.end(),
	                 [](const HintCandidate& lhs, const HintCandidate& rhs) {
		                 return lhs.score > rhs.score;
	                 });
}

}  // namespace

MinmaxHintEvaluation evaluateHintCandidatesWithMinmax(
    const Board& board,
    Side side,
    std::chrono::milliseconds time_limit)
{
	auto legal_moves = board.getAllLegalMoves(side);
	if (legal_moves.empty()) {
		throw std::runtime_error("No legal moves are available for hint.");
	}

	MinmaxHintEvaluation result;
	const auto deadline = Clock::now() + time_limit;
	const int max_depth = board.count(CellState::EMPTY);
	CellPosition best_move = legal_moves.front();

	for (int depth = 1; depth <= max_depth; ++depth) {
		std::vector<HintCandidate> iteration_candidates;
		auto moves_ordered = legal_moves;
		orderMoves(moves_ordered, best_move);

		bool completed = true;
		try {
			for (auto move : moves_ordered) {
				Board next = board;
				next.placeDisk(move, side);
				const int score = alphabeta(next, getOpponentSide(side), side,
				                            depth - 1,
				                            std::numeric_limits<int>::min() / 2,
				                            std::numeric_limits<int>::max() / 2,
				                            deadline,
				                            false);
				iteration_candidates.push_back({move, score, ""});
			}
		} catch (const TimeoutException&) {
			completed = false;
		}

		if (!completed) {
			break;
		}

		sortCandidates(iteration_candidates);
		result.candidates = iteration_candidates;
		result.completed_depth = depth;
		best_move = result.candidates.front().move;

		const int best_score = result.candidates.front().score;
		if (best_score >= WIN_SCORE || best_score <= -WIN_SCORE || Clock::now() >= deadline) {
			break;
		}
	}

	if (result.candidates.empty()) {
		result.candidates = evaluateOnePlyFallback(board, side, legal_moves);
		sortCandidates(result.candidates);
		result.completed_depth = 0;
	}

	return result;
}

}  // namespace reversi
