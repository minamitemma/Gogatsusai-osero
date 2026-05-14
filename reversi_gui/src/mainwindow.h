#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QLCDNumber>
#include <QTextEdit>
#include <QTimer>
#include <QVector>
#include "boardwidget.h"
#include "gamemanager.h"
#include "minmaxengine.h"

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

private:
    struct RankingEntry
    {
        QString name;
        int score;
        int blackCount;
        int whiteCount;
        QString playedAt;
    };

    void setupUI();
    void updateGameInfo();
    void startTimer();
    void stopTimer();
    void resetTimer();
    bool isAiTurn(const GameState &state) const;
    void scheduleAiMove();
    void makeAiMove();
    void makeRandomPlayerMove();
    void handleGameOver(const GameState &state);
    void updateRankingDisplay();
    void saveRanking(const RankingEntry &entry);
    QVector<RankingEntry> loadRankings() const;
    QString rankingFilePath() const;
    
    // UI Components
    BoardWidget *boardWidget;
    QLabel *currentPlayerLabel;
    QLabel *blackCountLabel;
    QLabel *whiteCountLabel;
    QLCDNumber *timerDisplay;
    QPushButton *hintButton;
    QPushButton *resetButton;
    QLabel *avatarLabel;
    QLabel *statusLabel;
    QTextEdit *hintTextDisplay;
    QTextEdit *rankingTextDisplay;
    
    // Game Management
    GameManager *gameManager;
    MinmaxEngine *hintEngine;
    int hintsRemaining;
    
    // Timer
    QTimer *gameTimer;
    int timeRemaining;
    bool isTimerActive;
    bool aiMovePending;
    bool rankingRecorded;
};

#endif // MAINWINDOW_H
