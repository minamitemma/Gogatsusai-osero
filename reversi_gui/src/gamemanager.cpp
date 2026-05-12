#include "gamemanager.h"

GameManager::GameManager(QObject *parent)
    : QObject(parent)
{
}

void GameManager::initializeGame()
{
    // Initialize 6x6 board
    currentState.board = std::vector<std::vector<int>>(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    
    // Starting position
    currentState.board[2][2] = 1; // Black
    currentState.board[2][3] = 2; // White
    currentState.board[3][2] = 2; // White
    currentState.board[3][3] = 1; // Black
    
    currentState.currentPlayer = 1; // Black starts
    currentState.gameOver = false;
    
    updateCounts();
    currentState.validMoves = getValidMoves(currentState.currentPlayer);
    
    emit gameStateChanged(currentState);
}

void GameManager::makeMove(int row, int col)
{
    if (!isValidMove(row, col, currentState.currentPlayer)) {
        emit moveError("Invalid move!");
        return;
    }
    
    currentState.board[row][col] = currentState.currentPlayer;
    flipStones(row, col, currentState.currentPlayer);
    
    updateCounts();
    switchPlayer();
    
    currentState.validMoves = getValidMoves(currentState.currentPlayer);
    if (currentState.validMoves.empty()) {
        // Current player has no valid moves, switch again
        switchPlayer();
        currentState.validMoves = getValidMoves(currentState.currentPlayer);
        
        if (currentState.validMoves.empty()) {
            // Game over
            currentState.gameOver = true;
        }
    }
    
    emit gameStateChanged(currentState);
}

bool GameManager::isValidMove(int row, int col, int player)
{
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        return false;
    }
    
    if (currentState.board[row][col] != 0) {
        return false;
    }
    
    // Check all 8 directions
    const int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    
    for (const auto &dir : directions) {
        if (countFlips(row, col, player, dir[0], dir[1]) > 0) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::pair<int, int>> GameManager::getValidMoves(int player)
{
    std::vector<std::pair<int, int>> moves;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (isValidMove(row, col, player)) {
                moves.push_back({row, col});
            }
        }
    }
    return moves;
}

void GameManager::updateCounts()
{
    int blackCount = 0, whiteCount = 0;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (currentState.board[row][col] == 1) {
                blackCount++;
            } else if (currentState.board[row][col] == 2) {
                whiteCount++;
            }
        }
    }
    currentState.blackCount = blackCount;
    currentState.whiteCount = whiteCount;
}

void GameManager::flipStones(int row, int col, int player)
{
    const int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    
    for (const auto &dir : directions) {
        int count = countFlips(row, col, player, dir[0], dir[1]);
        for (int i = 1; i <= count; ++i) {
            int r = row + i * dir[0];
            int c = col + i * dir[1];
            currentState.board[r][c] = player;
        }
    }
}

int GameManager::countFlips(int row, int col, int player, int dRow, int dCol)
{
    int opponent = (player == 1) ? 2 : 1;
    int count = 0;
    int r = row + dRow;
    int c = col + dCol;
    
    while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
        if (currentState.board[r][c] == 0) {
            return 0;
        }
        if (currentState.board[r][c] == opponent) {
            count++;
        } else {
            return count;
        }
        r += dRow;
        c += dCol;
    }
    
    return 0;
}

void GameManager::switchPlayer()
{
    currentState.currentPlayer = (currentState.currentPlayer == 1) ? 2 : 1;
}
