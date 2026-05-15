#include "review/game_reviewer.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace reversi
{

namespace
{

std::string toString(Side side)
{
	std::ostringstream output;
	output << side;
	return output.str();
}

std::string toString(CellPosition position)
{
	std::ostringstream output;
	output << position;
	return output.str();
}

std::string signedString(int value)
{
	return (value > 0 ? std::string("+") : std::string("")) + std::to_string(value);
}

std::string resultText(const GameRecord& record)
{
	if (record.final_black_count > record.final_white_count) {
		return "黒の勝ち";
	}
	if (record.final_black_count < record.final_white_count) {
		return "白の勝ち";
	}
	return "引き分け";
}

std::vector<MoveRecord> playedMoves(const GameRecord& record)
{
	std::vector<MoveRecord> moves;
	for (const auto& move : record.moves) {
		if (!move.was_pass) {
			moves.push_back(move);
		}
	}
	return moves;
}

void sortByDeltaDescending(std::vector<MoveRecord>& moves)
{
	std::stable_sort(moves.begin(), moves.end(),
	                 [](const MoveRecord& lhs, const MoveRecord& rhs) {
		                 return lhs.delta_for_mover > rhs.delta_for_mover;
	                 });
}

void appendMoveLine(std::ostringstream& output, const MoveRecord& move)
{
	output << move.turn_number << "手目 " << toString(move.side) << " " << toString(move.move)
	       << " (" << signedString(move.delta_for_mover) << ")";
}

}  // namespace

GameReviewer::GameReviewer(ReviewPromptBuilder prompt_builder)
    : m_prompt_builder(prompt_builder)
{
}

std::string GameReviewer::buildLocalReview(const GameRecord& record) const
{
	std::ostringstream output;
	output << "[game review]\n";
	output << "結果: " << resultText(record) << " (黒 " << record.final_black_count
	       << " - 白 " << record.final_white_count << ")\n";

	auto moves = playedMoves(record);
	if (moves.empty()) {
		output << "記録された着手がありません。\n";
		return output.str();
	}

	sortByDeltaDescending(moves);
	output << "良かった手: ";
	appendMoveLine(output, moves.front());
	output << "\n";

	output << "改善できそうな手: ";
	appendMoveLine(output, moves.back());
	output << "\n";

	output << "振り返り: 評価が大きく上がった手は、相手の選択肢を狭めたり、終盤の石差につながった可能性があります。";
	output << "逆に評価が下がった手は、次回は候補手のスコア差を見てから選ぶと安定します。\n";
	return output.str();
}

std::string GameReviewer::buildLlmReview(const GameRecord& record, const LlmClient& llm_client) const
{
	return llm_client.generateHint(m_prompt_builder.build(record));
}

}  // namespace reversi
