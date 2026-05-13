#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QLCDNumber>
#include <QTimer>
#include "boardwidget.h"
#include "gamemanager.h"
#include "simplehintengine.h"

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
    void setupUI();
    void updateGameInfo();
    void startTimer();
    void stopTimer();
    void resetTimer();
    
    // UI Components
    BoardWidget *boardWidget;
    QLabel *currentPlayerLabel;
    QLabel *blackCountLabel;
    QLabel *whiteCountLabel;
    QLCDNumber *timerDisplay;
    QPushButton *hintButton;
    QPushButton *resetButton;
    QLabel *avatarLabel;
    
    // Game Management
    GameManager *gameManager;
    SimpleHintEngine *hintEngine;
    int hintsRemaining;
    
    // Timer
    QTimer *gameTimer;
    int timeRemaining;
    bool isTimerActive;
};

#endif // MAINWINDOW_H
