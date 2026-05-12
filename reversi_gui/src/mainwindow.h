#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QLCDNumber>
#include "boardwidget.h"
#include "gamemanager.h"

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

private:
    void setupUI();
    void updateGameInfo();
    
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
    int hintsRemaining;
};

#endif // MAINWINDOW_H
