#include "minmaxengine.h"

#include <sstream>
#include <stdexcept>

#include "evaluator.hpp"
#include "hint/hint_formatter.hpp"
#include "hint/local_hint_engine.hpp"

MinmaxEngine::MinmaxEngine()
{
}

void MinmaxEngine::initialize()
{
}

void MinmaxEngine::shutdown()
{
}

std::pair<int, int> MinmaxEngine::getNextMove(
    const std::vector<std::vector<int>> &board,
    int player
)
{
    return getHint(board, player).move;
}

QString MinmaxEngine::getHintText(
    const std::vector<std::vector<int>> &board,
    int player
)
{
    return getHint(board, player).text;
}

QString MinmaxEngine::getHintJson(
    const std::vector<std::vector<int>> &board,
    int player
)
{
    return getHint(board, player).json;
}

MinmaxHintDisplay MinmaxEngine::getHint(
    const std::vector<std::vector<int>> &board,
    int player
)
{
    reversi::LocalHintEngine engine;
    const reversi::Side side = convertSide(player);
    const reversi::HintResult hint = engine.getHint(convertBoard(board), side);

    QString text;
    text += QString("おすすめ手: %1\n").arg(formatMove(hint.best_move));
    text += QString("形勢評価: %1\n").arg(hint.evaluation_score);
    text += QString("探索深さ: %1\n").arg(hint.search_depth);
    text += "候補手:\n";
    for (const auto &candidate : hint.candidates) {
        text += QString("- %1: %2\n").arg(formatMove(candidate.move)).arg(candidate.score);
    }
    text += QString("解説: %1").arg(QString::fromStdString(hint.explanation));

    return {
        {hint.best_move.y, hint.best_move.x},
        text,
        QString::fromStdString(reversi::formatHintJson(hint, side))
    };
}

int MinmaxEngine::evaluatePosition(
    const std::vector<std::vector<int>> &board,
    int player
) const
{
    return reversi::evaluate(convertBoard(board), convertSide(player));
}

reversi::Board MinmaxEngine::convertBoard(const std::vector<std::vector<int>> &board) const
{
    reversi::Board converted;
    for (int row = 0; row < reversi::Board::HEIGHT; ++row) {
        for (int col = 0; col < reversi::Board::WIDTH; ++col) {
            reversi::CellState state = reversi::CellState::EMPTY;
            if (row < static_cast<int>(board.size()) && col < static_cast<int>(board[row].size())) {
                if (board[row][col] == 1) {
                    state = reversi::CellState::BLACK;
                } else if (board[row][col] == 2) {
                    state = reversi::CellState::WHITE;
                }
            }
            converted.set({col, row}, state);
        }
    }
    return converted;
}

reversi::Side MinmaxEngine::convertSide(int player) const
{
    return player == 1 ? reversi::Side::BLACK : reversi::Side::WHITE;
}

QString MinmaxEngine::formatMove(reversi::CellPosition move) const
{
    std::ostringstream output;
    output << move;
    return QString::fromStdString(output.str());
}
