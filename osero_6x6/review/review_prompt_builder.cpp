#include "review/review_prompt_builder.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace reversi
{

namespace
{

std::string readTextFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file) {
		return "";
	}

	std::ostringstream content;
	content << file.rdbuf();
	return content.str();
}

std::string loadReviewPromptTemplate()
{
	const char* paths[] = {
	    "review_prompt.txt",
	    "../review_prompt.txt",
	    "osero_6x6/review_prompt.txt",
	};

	for (const auto path : paths) {
		const std::string content = readTextFile(path);
		if (!content.empty()) {
			return content;
		}
	}

	throw std::runtime_error("review_prompt.txt was not found.");
}

void replaceAll(std::string& text, const std::string& from, const std::string& to)
{
	std::size_t pos = 0;
	while ((pos = text.find(from, pos)) != std::string::npos) {
		text.replace(pos, from.size(), to);
		pos += to.size();
	}
}

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

std::string buildMoveLog(const GameRecord& record)
{
	std::ostringstream output;
	for (const auto& move : record.moves) {
		output << move.turn_number << ". " << toString(move.side) << " ";
		if (move.was_pass) {
			output << "pass";
		} else {
			output << toString(move.move);
		}
		output << " eval " << signedString(move.black_eval_before)
		       << " -> " << signedString(move.black_eval_after)
		       << ", mover delta " << signedString(move.delta_for_mover)
		       << ", stones B" << move.black_count_after
		       << "-W" << move.white_count_after << "\n";
	}
	return output.str();
}

std::string humanSideText(const GameRecord& record)
{
	if (!record.has_human_side) {
		return "none";
	}
	return toString(record.human_side);
}

}  // namespace

std::string ReviewPromptBuilder::build(const GameRecord& record) const
{
	std::string prompt = loadReviewPromptTemplate();
	replaceAll(prompt, "{{black_count}}", std::to_string(record.final_black_count));
	replaceAll(prompt, "{{white_count}}", std::to_string(record.final_white_count));
	replaceAll(prompt, "{{human_side}}", humanSideText(record));
	replaceAll(prompt, "{{move_log}}", buildMoveLog(record));
	return prompt;
}

}  // namespace reversi
