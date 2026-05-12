#pragma once

#include "board.hpp"

namespace reversi
{

// 終盤に切り替える残り空きマス数のしきい値
constexpr int ENDGAME_EMPTY_THRESHOLD = 10;

// side 視点の盤面評価値を返す。
// 中盤: 位置重み + 8 * (自分の合法手数 - 相手の合法手数)
// 終盤 (空き <= ENDGAME_EMPTY_THRESHOLD): 1000 * 石差
// 終局 (空き = 0): ±1,000,000 + 石差 (勝敗確定)
int evaluate(const Board& board, Side side);

// (x, y) マスの位置重み (角=100, X-square=-50, など)
int positionWeight(int x, int y);

}  // namespace reversi
