#include "hint/hint_formatter.hpp"

#include <sstream>
#include <string>

namespace reversi
{

namespace
{

std::string toString(CellPosition position)
{
	std::ostringstream output;
	output << position;
	return output.str();
}

std::string toString(Side side)
{
	std::ostringstream output;
	output << side;
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

}  // namespace

std::string formatHintText(const HintResult& hint)
{
	std::ostringstream output;
	output << "おすすめ手: " << hint.best_move << "\n";
	output << "形勢評価: " << hint.evaluation_score << "\n";
	output << "探索深さ: " << hint.search_depth << "\n";
	output << "候補手:\n";
	for (const auto& candidate : hint.candidates) {
		output << "- " << candidate.move << ": " << candidate.score << "\n";
	}
	output << "解説: " << hint.explanation << "\n";
	return output.str();
}

std::string formatHintJson(const HintResult& hint, Side side)
{
	std::ostringstream output;
	output << "{\n";
	output << "  \"side\": \"" << jsonEscape(toString(side)) << "\",\n";
	output << "  \"bestMove\": \"" << jsonEscape(toString(hint.best_move)) << "\",\n";
	output << "  \"evaluationScore\": " << hint.evaluation_score << ",\n";
	output << "  \"searchDepth\": " << hint.search_depth << ",\n";
	output << "  \"positionSummary\": \"" << jsonEscape(hint.position_summary) << "\",\n";
	output << "  \"explanation\": \"" << jsonEscape(hint.explanation) << "\",\n";
	output << "  \"candidates\": [\n";
	for (std::size_t i = 0; i < hint.candidates.size(); ++i) {
		const auto& candidate = hint.candidates.at(i);
		output << "    {";
		output << "\"move\": \"" << jsonEscape(toString(candidate.move)) << "\", ";
		output << "\"score\": " << candidate.score;
		if (!candidate.reason.empty()) {
			output << ", \"reason\": \"" << jsonEscape(candidate.reason) << "\"";
		}
		output << "}";
		if (i + 1 < hint.candidates.size()) {
			output << ",";
		}
		output << "\n";
	}
	output << "  ]\n";
	output << "}";
	return output.str();
}

}  // namespace reversi
