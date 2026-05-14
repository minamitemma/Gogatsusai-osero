#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <QTimer>
#include <exception>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), hintsRemaining(3), timeRemaining(10), isTimerActive(false), aiMovePending(false)
{
    setupUI();
    gameManager = new GameManager(this);
    hintEngine = new MinmaxEngine();
    hintEngine->initialize();
    
    // Setup timer
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::onTimerTimeout);
    
    // Connect signals
    connect(boardWidget, &BoardWidget::cellClicked, this, &MainWindow::onBoardClicked);
    connect(hintButton, &QPushButton::clicked, this, &MainWindow::onHintButtonClicked);
    connect(gameManager, &GameManager::gameStateChanged, this, &MainWindow::onGameStateChanged);
    
    // Initialize game
    gameManager->initializeGame();
    resetTimer();
}

MainWindow::~MainWindow()
{
    if (hintEngine) {
        hintEngine->shutdown();
        delete hintEngine;
    }
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
        aiMovePending = false;
        hintButton->setText(QString("Hint (%1/3)").arg(hintsRemaining));
        stopTimer();
        gameManager->initializeGame();
        resetTimer();
    });
    controlLayout->addWidget(resetButton);
    
    controlLayout->addStretch();
    
    mainLayout->addWidget(controlGroupBox);
    
    setCentralWidget(centralWidget);
}

void MainWindow::onBoardClicked(int row, int col)
{
    GameState state = gameManager->getGameState();
    if (state.gameOver || isAiTurn(state)) {
        return;
    }

    if (isTimerActive) {
        stopTimer();
    }
    
    gameManager->makeMove(row, col);
}

void MainWindow::onHintButtonClicked()
{
    GameState state = gameManager->getGameState();
    if (state.gameOver || isAiTurn(state)) {
        return;
    }

    if (hintsRemaining > 0 && hintEngine) {
        hintsRemaining--;
        hintButton->setText(QString("Hint (%1/3)").arg(hintsRemaining));
        
        MinmaxHintDisplay hint;
        try {
            hint = hintEngine->getHint(state.board, state.currentPlayer);
        } catch (const std::exception &error) {
            QMessageBox::warning(this, "Hint", error.what());
            return;
        }

        std::cout << hint.json.toStdString() << std::endl;

        auto [hintRow, hintCol] = hint.move;
        
        // Highlight the suggested move on the board
        std::vector<std::pair<int, int>> hintMoves = {{hintRow, hintCol}};
        boardWidget->setValidMoves(hintMoves);
        
        // Show hint message
        QString playerColor = (state.currentPlayer == 1) ? "Black" : "White";
        QString message = QString("Hint for %1: Row %2, Column %3")
                              .arg(playerColor)
                              .arg(hintRow + 1)
                              .arg(hintCol + 1);
        QMessageBox::information(this, "Hint", hint.text);
        
        // TODO: Show message in a dialog or status bar
        // For now, just update the current player label temporarily
        QString originalText = currentPlayerLabel->text();
        currentPlayerLabel->setText(message);
        
        // Reset after 3 seconds
        QTimer::singleShot(3000, [this, originalText]() {
            currentPlayerLabel->setText(originalText);
            // Reset valid moves display
            GameState currentState = gameManager->getGameState();
            boardWidget->setValidMoves(currentState.validMoves);
        });
    }
}

void MainWindow::onGameStateChanged(const GameState &state)
{
    boardWidget->updateBoard(state.board);
    boardWidget->setValidMoves(state.validMoves);
    updateGameInfo();
    
    // Handle game over
    if (state.gameOver) {
        stopTimer();
        timerDisplay->display("00");
        hintButton->setEnabled(false);
    } else if (isAiTurn(state)) {
        stopTimer();
        hintButton->setEnabled(false);
        scheduleAiMove();
    } else {
        aiMovePending = false;
        hintButton->setEnabled(hintsRemaining > 0);
        resetTimer();
        startTimer();
    }
}

void MainWindow::onTimerTick()
{
    // This method is kept for future use if we need more frequent updates
}

void MainWindow::onTimerTimeout()
{
    timeRemaining--;
    timerDisplay->display(QString("%1").arg(timeRemaining, 2, 10, QChar('0')));
    
    if (timeRemaining <= 0) {
        // Time's up - auto pass or end turn
        stopTimer();
        
        GameState state = gameManager->getGameState();
        if (!state.gameOver && !isAiTurn(state)) {
            // Auto pass - switch to next player
            gameManager->makeMove(-1, -1); // Special move to pass
        }
    } else {
        // Continue timer
        gameTimer->start(1000); // 1 second intervals
    }
}

void MainWindow::startTimer()
{
    if (!isTimerActive && timeRemaining > 0) {
        isTimerActive = true;
        gameTimer->start(1000); // 1 second intervals
    }
}

void MainWindow::stopTimer()
{
    if (isTimerActive) {
        isTimerActive = false;
        gameTimer->stop();
    }
}

void MainWindow::resetTimer()
{
    stopTimer();
    timeRemaining = 10;
    timerDisplay->display("10");
}

bool MainWindow::isAiTurn(const GameState &state) const
{
    return state.currentPlayer == 2;
}

void MainWindow::scheduleAiMove()
{
    if (aiMovePending) {
        return;
    }

    aiMovePending = true;
    QTimer::singleShot(500, this, &MainWindow::makeAiMove);
}

void MainWindow::makeAiMove()
{
    aiMovePending = false;

    GameState state = gameManager->getGameState();
    if (state.gameOver || !isAiTurn(state)) {
        return;
    }

    std::pair<int, int> move;
    try {
        move = hintEngine->getNextMove(state.board, state.currentPlayer);
    } catch (const std::exception &) {
        gameManager->makeMove(-1, -1);
        return;
    }

    auto [row, col] = move;
    if (row < 0 || col < 0) {
        gameManager->makeMove(-1, -1);
        return;
    }

    gameManager->makeMove(row, col);
}

void MainWindow::updateGameInfo()
{
    GameState state = gameManager->getGameState();
    
    // Update player turn
    QString playerText;
    if (state.gameOver) {
        QString winner;
        if (state.blackCount > state.whiteCount) {
            winner = QString("Black wins by %1!").arg(state.blackCount - state.whiteCount);
        } else if (state.whiteCount > state.blackCount) {
            winner = QString("White wins by %1!").arg(state.whiteCount - state.blackCount);
        } else {
            winner = "Draw!";
        }
        playerText = winner;
    } else {
        playerText = QString("Current Player: %1").arg((state.currentPlayer == 1) ? "Black" : "White");
    }
    currentPlayerLabel->setText(playerText);
    
    // Update stone counts
    blackCountLabel->setText(QString("Black: %1").arg(state.blackCount));
    whiteCountLabel->setText(QString("White: %1").arg(state.whiteCount));
    
    // Update avatar based on evaluation (simplified)
    if (state.gameOver) {
        if (state.blackCount > state.whiteCount) {
            avatarLabel->setText("🏆"); // Black wins
        } else if (state.whiteCount > state.blackCount) {
            avatarLabel->setText("🏆"); // White wins
        } else {
            avatarLabel->setText("🤝"); // Draw
        }
    } else {
        int diff = state.blackCount - state.whiteCount;
        if (diff > 5) {
            avatarLabel->setText("😄"); // Black is winning
        } else if (diff < -5) {
            avatarLabel->setText("😢"); // Black is losing
        } else {
            avatarLabel->setText("😊"); // Balanced
        }
    }
}
