#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QGroupBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), hintsRemaining(3)
{
    setupUI();
    gameManager = new GameManager(this);
    
    // Connect signals
    connect(boardWidget, &BoardWidget::cellClicked, this, &MainWindow::onBoardClicked);
    connect(hintButton, &QPushButton::clicked, this, &MainWindow::onHintButtonClicked);
    connect(gameManager, &GameManager::gameStateChanged, this, &MainWindow::onGameStateChanged);
    
    // Initialize game
    gameManager->initializeGame();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Top: Game Info Section
    QGroupBox *infoGroupBox = new QGroupBox("Game Information", this);
    QHBoxLayout *infoLayout = new QHBoxLayout(infoGroupBox);
    
    // Player turn
    currentPlayerLabel = new QLabel("Current Player: Black", this);
    infoLayout->addWidget(currentPlayerLabel);
    
    // Stone counts
    blackCountLabel = new QLabel("Black: 2", this);
    whiteCountLabel = new QLabel("White: 2", this);
    infoLayout->addWidget(blackCountLabel);
    infoLayout->addWidget(whiteCountLabel);
    
    // Timer
    timerDisplay = new QLCDNumber(2, this);
    timerDisplay->setSegmentStyle(QLCDNumber::Flat);
    timerDisplay->display("10");
    infoLayout->addWidget(new QLabel("Time: ", this));
    infoLayout->addWidget(timerDisplay);
    
    mainLayout->addWidget(infoGroupBox);
    
    // Middle: Board Display
    boardWidget = new BoardWidget(this);
    boardWidget->setMinimumSize(400, 400);
    mainLayout->addWidget(boardWidget);
    
    // Bottom: Control Section
    QGroupBox *controlGroupBox = new QGroupBox("Controls", this);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroupBox);
    
    // Avatar
    avatarLabel = new QLabel(this);
    avatarLabel->setText("😊");
    avatarLabel->setStyleSheet("font-size: 48px;");
    avatarLabel->setFixedSize(60, 60);
    avatarLabel->setAlignment(Qt::AlignCenter);
    controlLayout->addWidget(avatarLabel);
    
    // Hint Button
    hintButton = new QPushButton("Hint (3/3)", this);
    controlLayout->addWidget(hintButton);
    
    // Reset Button
    resetButton = new QPushButton("New Game", this);
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        hintsRemaining = 3;
        hintButton->setText(QString("Hint (%1/3)").arg(hintsRemaining));
        gameManager->initializeGame();
    });
    controlLayout->addWidget(resetButton);
    
    controlLayout->addStretch();
    
    mainLayout->addWidget(controlGroupBox);
    
    setCentralWidget(centralWidget);
}

void MainWindow::onBoardClicked(int row, int col)
{
    gameManager->makeMove(row, col);
}

void MainWindow::onHintButtonClicked()
{
    if (hintsRemaining > 0) {
        hintsRemaining--;
        hintButton->setText(QString("Hint (%1/3)").arg(hintsRemaining));
        // TODO: Call hint API
    }
}

void MainWindow::onGameStateChanged(const GameState &state)
{
    boardWidget->updateBoard(state.board);
    boardWidget->setValidMoves(state.validMoves);
    updateGameInfo();
}

void MainWindow::onTimerTick()
{
    // TODO: Update timer display
}

void MainWindow::updateGameInfo()
{
    GameState state = gameManager->getGameState();
    
    // Update player turn
    QString playerText = (state.currentPlayer == 1) ? "Black" : "White";
    currentPlayerLabel->setText(QString("Current Player: %1").arg(playerText));
    
    // Update stone counts
    blackCountLabel->setText(QString("Black: %1").arg(state.blackCount));
    whiteCountLabel->setText(QString("White: %1").arg(state.whiteCount));
    
    // Update avatar based on evaluation (simplified)
    int diff = state.blackCount - state.whiteCount;
    if (diff > 5) {
        avatarLabel->setText("😄"); // Black is winning
    } else if (diff < -5) {
        avatarLabel->setText("😢"); // Black is losing
    } else {
        avatarLabel->setText("😊"); // Balanced
    }
    
    // Update game over status
    if (state.gameOver) {
        QString winner;
        if (state.blackCount > state.whiteCount) {
            winner = QString("Black wins by %1!").arg(state.blackCount - state.whiteCount);
        } else if (state.whiteCount > state.blackCount) {
            winner = QString("White wins by %1!").arg(state.whiteCount - state.blackCount);
        } else {
            winner = "Draw!";
        }
        currentPlayerLabel->setText(winner);
    }
}
