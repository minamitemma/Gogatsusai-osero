#ifndef MINMAXENGINE_H
#define MINMAXENGINE_H

#include "aiengine.h"

#include <QString>
#include <string>

#include "board.hpp"

struct MinmaxHintDisplay
{
    std::pair<int, int> move;
    QString text;
    QString json;
};

class MinmaxEngine : public IAIEngine
{
public:
    MinmaxEngine();

    std::pair<int, int> getNextMove(
        const std::vector<std::vector<int>> &board,
        int player
    ) override;

    QString getHintText(
        const std::vector<std::vector<int>> &board,
        int player
    );

    QString getHintJson(
        const std::vector<std::vector<int>> &board,
        int player
    );

    MinmaxHintDisplay getHint(
        const std::vector<std::vector<int>> &board,
        int player
    );

    int evaluatePosition(
        const std::vector<std::vector<int>> &board,
        int player
    ) const;

    void initialize() override;
    void shutdown() override;

private:
    reversi::Board convertBoard(const std::vector<std::vector<int>> &board) const;
    reversi::Side convertSide(int player) const;
    QString formatMove(reversi::CellPosition move) const;
};

#endif // MINMAXENGINE_H
