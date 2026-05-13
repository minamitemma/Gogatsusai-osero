#include "hint/local_hint_engine.hpp"

#include <sstream>
#include <stdexcept>

#include "evaluator.hpp"
#include "hint/minmax_hint_evaluator.hpp"

namespace reversi
{

HintResult LocalHintEngine::getHint(const Board& board, Side side)
{
	const auto legal_moves = board.getAllLegalMoves(side);
	if (legal_moves.empty()) {
		throw std::runtime_error("No legal moves are available for hint.");
	}

	HintResult result{};
	result.evaluation_score = evaluate(board, side);
	const MinmaxHintEvaluation minmax_result = evaluateHintCandidatesWithMinmax(board, side);
	result.candidates = minmax_result.candidates;
	result.search_depth = minmax_result.completed_depth;
	result.best_move = result.candidates.front().move;

	std::ostringstream summary;
	summary << side << " perspective score: " << result.evaluation_score
	        << ", minmax depth: " << result.search_depth;
	result.position_summary = summary.str();

	std::ostringstream explanation;
	explanation << "Minmax recommends " << result.best_move
	            << " with score " << result.candidates.front().score
	            << " at depth " << result.search_depth << ".";
	result.explanation = explanation.str();

	return result;
}

}  // namespace reversi
