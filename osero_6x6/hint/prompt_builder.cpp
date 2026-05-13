#include "hint/prompt_builder.hpp"

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

std::string loadPromptTemplate()
{
	const char* paths[] = {
	    "prompt.txt",
	    "../prompt.txt",
	    "osero_6x6/prompt.txt",
	};

	for (const auto path : paths) {
		const std::string content = readTextFile(path);
		if (!content.empty()) {
			return content;
		}
	}

	throw std::runtime_error("prompt.txt was not found.");
}

void replaceAll(std::string& text, const std::string& from, const std::string& to)
{
	std::size_t pos = 0;
	while ((pos = text.find(from, pos)) != std::string::npos) {
		text.replace(pos, from.size(), to);
		pos += to.size();
	}
}

std::string toString(const Board& board)
{
	std::ostringstream output;
	output << board;
	return output.str();
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

std::string buildCandidateText(const HintResult& local_hint)
{
	std::ostringstream candidates;
	for (const auto& candidate : local_hint.candidates) {
		candidates << "- " << candidate.move << ": score " << candidate.score << "\n";
	}
	return candidates.str();
}

}  // namespace

std::string PromptBuilder::build(const Board& board, Side side, const HintResult& local_hint) const
{
	std::string prompt = loadPromptTemplate();

	replaceAll(prompt, "{{side}}", toString(side));
	replaceAll(prompt, "{{board}}", toString(board));
	replaceAll(prompt, "{{best_move}}", toString(local_hint.best_move));
	replaceAll(prompt, "{{evaluation_score}}", std::to_string(local_hint.evaluation_score));
	replaceAll(prompt, "{{search_depth}}", std::to_string(local_hint.search_depth));
	replaceAll(prompt, "{{candidates}}", buildCandidateText(local_hint));

	return prompt;
}

}  // namespace reversi
