#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <vector>

struct GameState {
    std::vector<std::vector<int>> board;
    int blackCount;
    int whiteCount;
    int currentPlayer; // 1: Black, 2: White
    std::vector<std::pair<int, int>> validMoves;
    bool gameOver;
};

class GameManager : public QObject
{
    Q_OBJECT

public:
    GameManager(QObject *parent = nullptr);
    
    void initializeGame();
    void makeMove(int row, int col);
    GameState getGameState() const { return currentState; }
    
signals:
    void gameStateChanged(const GameState &state);
    void moveError(const QString &message);

private:
    bool isValidMove(int row, int col, int player);
    std::vector<std::pair<int, int>> getValidMoves(int player);
    void updateCounts();
    void flipStones(int row, int col, int player);
    int countFlips(int row, int col, int player, int dRow, int dCol);
    void switchPlayer();
    
    static constexpr int BOARD_SIZE = 6;
    GameState currentState;
};

#endif // GAMEMANAGER_H
