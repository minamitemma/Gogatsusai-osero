#include "review/review_formatter.hpp"

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

std::string jsonEscape(const std::string& value)
{
	std::ostringstream escaped;
	for (const char raw_ch : value) {
		const unsigned char ch = static_cast<unsigned char>(raw_ch);
		switch (ch) {
		case '\\':
			escaped << "\\\\";
			break;
		case '"':
			escaped << "\\\"";
			break;
		case '\b':
			escaped << "\\b";
			break;
		case '\f':
			escaped << "\\f";
			break;
		case '\n':
			escaped << "\\n";
			break;
		case '\r':
			escaped << "\\r";
			break;
		case '\t':
			escaped << "\\t";
			break;
		default:
			if (ch < 0x20) {
				const char* digits = "0123456789abcdef";
				escaped << "\\u00" << digits[(ch >> 4) & 0x0f] << digits[ch & 0x0f];
			} else {
				escaped << raw_ch;
			}
			break;
		}
	}
	return escaped.str();
}

std::string winner(const GameRecord& record)
{
	if (record.final_black_count > record.final_white_count) {
		return "black";
	}
	if (record.final_black_count < record.final_white_count) {
		return "white";
	}
	return "draw";
}

std::string humanSide(const GameRecord& record)
{
	if (!record.has_human_side) {
		return "none";
	}
	return toString(record.human_side);
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

void writeMoveObject(std::ostringstream& output, const MoveRecord& move, int indent)
{
	const std::string space(static_cast<std::size_t>(indent), ' ');
	output << space << "{\n";
	output << space << "  \"turn\": " << move.turn_number << ",\n";
	output << space << "  \"side\": \"" << jsonEscape(toString(move.side)) << "\",\n";
	output << space << "  \"wasPass\": " << (move.was_pass ? "true" : "false") << ",\n";
	if (move.was_pass) {
		output << space << "  \"move\": null,\n";
	} else {
		output << space << "  \"move\": \"" << jsonEscape(toString(move.move)) << "\",\n";
	}
	output << space << "  \"blackEvalBefore\": " << move.black_eval_before << ",\n";
	output << space << "  \"blackEvalAfter\": " << move.black_eval_after << ",\n";
	output << space << "  \"deltaForMover\": " << move.delta_for_mover << ",\n";
	output << space << "  \"blackCountAfter\": " << move.black_count_after << ",\n";
	output << space << "  \"whiteCountAfter\": " << move.white_count_after << "\n";
	output << space << "}";
}

void writeNullableMoveObject(std::ostringstream& output, const std::vector<MoveRecord>& moves, bool best)
{
	if (moves.empty()) {
		output << "null";
		return;
	}
	writeMoveObject(output, best ? moves.front() : moves.back(), 4);
}

}  // namespace

std::string formatReviewJson(const GameRecord& record, const std::string& summary)
{
	auto moves_by_delta = playedMoves(record);
	sortByDeltaDescending(moves_by_delta);

	std::ostringstream output;
	output << "{\n";
	output << "  \"humanSide\": \"" << jsonEscape(humanSide(record)) << "\",\n";
	output << "  \"result\": {\n";
	output << "    \"winner\": \"" << winner(record) << "\",\n";
	output << "    \"blackCount\": " << record.final_black_count << ",\n";
	output << "    \"whiteCount\": " << record.final_white_count << "\n";
	output << "  },\n";
	output << "  \"bestMove\": ";
	writeNullableMoveObject(output, moves_by_delta, true);
	output << ",\n";
	output << "  \"worstMove\": ";
	writeNullableMoveObject(output, moves_by_delta, false);
	output << ",\n";
	output << "  \"summary\": \"" << jsonEscape(summary) << "\",\n";
	output << "  \"moves\": [\n";
	for (std::size_t i = 0; i < record.moves.size(); ++i) {
		writeMoveObject(output, record.moves.at(i), 4);
		if (i + 1 < record.moves.size()) {
			output << ",";
		}
		output << "\n";
	}
	output << "  ]\n";
	output << "}";
	return output.str();
}

}  // namespace reversi
