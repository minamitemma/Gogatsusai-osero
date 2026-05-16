#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QLCDNumber>
#include <QTextEdit>
#include <QTimer>
#include <QVector>
#include "boardwidget.h"
#include "gamemanager.h"
#include "minmaxengine.h"
#include "review/game_record.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBoardClicked(int row, int col);
    void onHintButtonClicked();
    void onGameStateChanged(const GameState &state);
    void onTimerTick();
    void onTimerTimeout();
    void onHintLoadingTick();

private:
    struct RankingEntry
    {
        QString name;
        int score;
        int blackCount;
        int whiteCount;
        int playerSide;
        QString playedAt;
    };

    void setupUI();
    bool choosePlayerSide();
    void startNewGame();
    void updateGameInfo();
    void startTimer();
    void stopTimer();
    void resetTimer();
    bool isAiTurn(const GameState &state) const;
    void scheduleAiMove();
    void makeAiMove();
    void makeRandomPlayerMove();
    void handleGameOver(const GameState &state);
    void recordMove(const GameState &beforeState, int row, int col);
    void resetGameRecord();
    void showGameReview(const QString &playerName, const GameState &state);
    void showHintResult(const MinmaxHintDisplay &hint);
    void showHintError(const QString &message);
    void updateRankingDisplay();
    void saveRanking(const RankingEntry &entry);
    QVector<RankingEntry> loadRankings() const;
    QString rankingFilePath() const;
    QString playerSideName() const;
    
    // UI Components
    BoardWidget *boardWidget;
    QLabel *currentPlayerLabel;
    QLabel *blackCountLabel;
    QLabel *whiteCountLabel;
    QLCDNumber *timerDisplay;
    QPushButton *hintButton;
    QPushButton *resetButton;
    QComboBox *sideComboBox;
    QLabel *avatarLabel;
    QLabel *statusLabel;
    QTextEdit *hintTextDisplay;
    QTextEdit *rankingTextDisplay;
    
    // Game Management
    GameManager *gameManager;
    MinmaxEngine *hintEngine;
    int hintsRemaining;
    int humanPlayer;
    reversi::GameRecord gameRecord;
    int turnNumber;
    
    // Timer
    QTimer *gameTimer;
    int timeRemaining;
    bool isTimerActive;
    bool aiMovePending;
    bool rankingRecorded;
    bool hintPending;
    QTimer *hintLoadingTimer;
    int hintLoadingDots;
};

#endif // MAINWINDOW_H
