#include "minmaxengine.h"

#include <sstream>
#include <stdexcept>

#include "evaluator.hpp"
#include "hint/hint_formatter.hpp"
#include "hint/llm_client_factory.hpp"
#include "hint/llm_hint_engine.hpp"
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

    return formatHintDisplay(hint, side, "MIN-MAX HINT");
}

MinmaxHintDisplay MinmaxEngine::getLlmHint(
    const std::vector<std::vector<int>> &board,
    int player
)
{
    reversi::LlmHintEngine engine(reversi::makeConfiguredLlmClient());
    const reversi::Side side = convertSide(player);
    const reversi::HintResult hint = engine.getHint(convertBoard(board), side);

    return formatHintDisplay(hint, side, QString::fromStdString(reversi::configuredLlmProviderLabel()));
}

MinmaxHintDisplay MinmaxEngine::formatHintDisplay(
    const reversi::HintResult &hint,
    reversi::Side side,
    const QString &label
) const
{
    const QString bestMove = formatMove(hint.best_move);
    const QString scoreColor = hint.evaluation_score >= 0 ? "#1f7a4d" : "#b9442f";

    QString text;
    text += "<div style='font-family: sans-serif; color: #26322d;'>";
    text += QString("<div style='font-size: 13px; font-weight: 700; color: #1f5f49;'>%1</div>").arg(label);
    text += QString(
                "<div style='margin: 6px 0 10px 0;'>"
                "<span style='font-size: 18px; font-weight: 800;'>おすすめ手</span> "
                "<span style='font-size: 30px; font-weight: 900; color: #a46a16;'>%1</span>"
                "</div>"
            ).arg(bestMove);
    text += QString(
                "<table cellspacing='0' cellpadding='6' style='margin-bottom: 10px;'>"
                "<tr>"
                "<td style='font-weight: 800; color: #4b5a51;'>形勢評価</td>"
                "<td style='font-size: 18px; font-weight: 900; color: %1;'>%2</td>"
                "<td style='width: 18px;'></td>"
                "<td style='font-weight: 800; color: #4b5a51;'>探索深さ</td>"
                "<td style='font-size: 18px; font-weight: 900;'>%3</td>"
                "</tr>"
                "</table>"
            ).arg(scoreColor).arg(hint.evaluation_score).arg(hint.search_depth);

    text += "<div style='font-weight: 800; margin: 6px 0;'>候補手ランキング</div>";
    text += "<table cellspacing='0' cellpadding='6' style='border-collapse: collapse; width: 100%;'>";
    text += "<tr style='background: #e8f3eb; color: #1f5f49; font-weight: 800;'>"
            "<th align='left'>順位</th><th align='left'>手</th><th align='right'>score</th></tr>";
    int rank = 1;
    for (const auto &candidate : hint.candidates) {
        const QString background = (rank % 2 == 0) ? "#fffaf0" : "#ffffff";
        text += QString(
                    "<tr style='background: %1;'>"
                    "<td>%2</td><td style='font-weight: 800;'>%3</td><td align='right'>%4</td>"
                    "</tr>"
                )
                    .arg(background)
                    .arg(rank)
                    .arg(formatMove(candidate.move))
                    .arg(candidate.score);
        ++rank;
    }
    text += "</table>";

    text += QString(
                "<div style='margin-top: 10px; padding: 8px; background: #fff4cf; border-radius: 6px;'>"
                "<b>解説:</b> %1"
                "</div>"
            ).arg(QString::fromStdString(hint.explanation).toHtmlEscaped());
    text += "</div>";

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
