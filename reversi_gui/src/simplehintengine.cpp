#include "simplehintengine.h"
#include <algorithm>

SimpleHintEngine::SimpleHintEngine()
{
}

void SimpleHintEngine::initialize()
{
    // Initialize if needed
}

void SimpleHintEngine::shutdown()
{
    // Cleanup if needed
}

std::pair<int, int> SimpleHintEngine::getNextMove(
    const std::vector<std::vector<int>> &board,
    int player
)
{
    std::pair<int, int> bestMove = {-1, -1};
    int bestScore = -1;
    
    const int BOARD_SIZE = 6;
    
    // Find all valid moves and their scores
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (board[row][col] == 0) {
                int score = evaluateMove(board, row, col, player);
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = {row, col};
                }
            }
        }
    }
    
    return bestMove;
}

int SimpleHintEngine::evaluateMove(
    const std::vector<std::vector<int>> &board,
    int row, int col, int player
)
{
    int score = 0;
    
    const int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    
    for (const auto &dir : directions) {
        score += countFlips(board, row, col, player, dir[0], dir[1]);
    }
    
    return score;
}

int SimpleHintEngine::countFlips(
    const std::vector<std::vector<int>> &board,
    int row, int col, int player,
    int dRow, int dCol
)
{
    int opponent = (player == 1) ? 2 : 1;
    int count = 0;
    int r = row + dRow;
    int c = col + dCol;
    
    const int BOARD_SIZE = 6;
    
    while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
        if (board[r][c] == 0) {
            return 0;
        }
        if (board[r][c] == opponent) {
            count++;
        } else {
            return count;
        }
        r += dRow;
        c += dCol;
    }
    
    return 0;
}
