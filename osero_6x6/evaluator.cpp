#include <array>

#include "evaluator.hpp"

namespace reversi
{

namespace
{

// 6x6 用の位置重み付け表
//   行(y) ↓  列(x) →
//      a    b    c    d    e    f
// 1  100  -20   10   10  -20  100
// 2  -20  -50   -2   -2  -50  -20
// 3   10   -2    1    1   -2   10
// 4   10   -2    1    1   -2   10
// 5  -20  -50   -2   -2  -50  -20
// 6  100  -20   10   10  -20  100
constexpr std::array<std::array<int, 6>, 6> POSITION_WEIGHT = {{
    {{100, -20,  10,  10, -20, 100}},
    {{-20, -50,  -2,  -2, -50, -20}},
    {{ 10,  -2,   1,   1,  -2,  10}},
    {{ 10,  -2,   1,   1,  -2,  10}},
    {{-20, -50,  -2,  -2, -50, -20}},
    {{100, -20,  10,  10, -20, 100}},
}};

constexpr int MOBILITY_WEIGHT = 8;
constexpr int ENDGAME_STONE_WEIGHT = 1000;
constexpr int WIN_SCORE = 1000000;

int positionScore(const Board& board, Side side)
{
	const CellState own = getOwnState(side);
	const CellState opp = getOwnState(getOpponentSide(side));
	int score = 0;
	for (int y = 0; y < Board::HEIGHT; ++y) {
		for (int x = 0; x < Board::WIDTH; ++x) {
			CellState s = board.get({x, y});
			if (s == own) {
				score += POSITION_WEIGHT[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
			} else if (s == opp) {
				score -= POSITION_WEIGHT[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
			}
		}
	}
	return score;
}

}  // namespace

int positionWeight(int x, int y)
{
	return POSITION_WEIGHT[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}

int evaluate(const Board& board, Side side)
{
	const int empty = board.count(CellState::EMPTY);
	const int own_count = board.count(getOwnState(side));
	const int opp_count = board.count(getOwnState(getOpponentSide(side)));

	if (empty == 0) {
		if (own_count > opp_count) return WIN_SCORE + (own_count - opp_count);
		if (own_count < opp_count) return -WIN_SCORE + (own_count - opp_count);
		return 0;
	}

	if (empty <= ENDGAME_EMPTY_THRESHOLD) {
		return ENDGAME_STONE_WEIGHT * (own_count - opp_count);
	}

	int pos = positionScore(board, side);
	int own_mobility = static_cast<int>(board.getAllLegalMoves(side).size());
	int opp_mobility = static_cast<int>(board.getAllLegalMoves(getOpponentSide(side)).size());
	return pos + MOBILITY_WEIGHT * (own_mobility - opp_mobility);
}

}  // namespace reversi
