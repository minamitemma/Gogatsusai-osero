#include "mainwindow.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <exception>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      hintsRemaining(3),
      timeRemaining(10),
      isTimerActive(false),
      aiMovePending(false),
      rankingRecorded(false)
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
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(14);

    centralWidget->setStyleSheet(
        "QWidget { background: #f6efe4; color: #26322d; font-size: 15px; }"
        "QGroupBox { background: #fffaf0; border: 2px solid #2f7d5c; border-radius: 8px; "
        "margin-top: 10px; padding: 12px; font-weight: 700; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }"
        "QPushButton { background: #ffcb45; border: 2px solid #a46a16; border-radius: 8px; "
        "padding: 10px 16px; font-weight: 700; }"
        "QPushButton:hover { background: #ffd86b; }"
        "QPushButton:disabled { background: #d6d0c4; border-color: #aaa397; color: #706a60; }"
        "QTextEdit { background: #ffffff; border: 2px solid #2f7d5c; border-radius: 8px; padding: 8px; }"
        "QLCDNumber { background: #26322d; color: #ffcb45; border-radius: 6px; }"
    );

    QLabel *titleLabel = new QLabel("6x6 Reversi Challenge", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 26px; font-weight: 800; color: #1f5f49; background: transparent;");
    mainLayout->addWidget(titleLabel);
    
    // Top: Game Info Section
    QGroupBox *infoGroupBox = new QGroupBox("Game Information", this);
    QHBoxLayout *infoLayout = new QHBoxLayout(infoGroupBox);
    
    // Player turn
    currentPlayerLabel = new QLabel("Current Player: Black", this);
    currentPlayerLabel->setStyleSheet("font-size: 18px; font-weight: 800; background: transparent;");
    infoLayout->addWidget(currentPlayerLabel);
    
    // Stone counts
    blackCountLabel = new QLabel("Black: 2", this);
    whiteCountLabel = new QLabel("White: 2", this);
    blackCountLabel->setStyleSheet("background: transparent;");
    whiteCountLabel->setStyleSheet("background: transparent;");
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
    avatarLabel->setStyleSheet("font-size: 48px; background: transparent;");
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
        rankingRecorded = false;
        hintButton->setText(QString("Hint (%1/3)").arg(hintsRemaining));
        hintTextDisplay->setPlainText("ヒントボタンを押すと、min-max のおすすめ手がここに表示されます。");
        statusLabel->setText("New game started. You are black.");
        stopTimer();
        gameManager->initializeGame();
        resetTimer();
    });
    controlLayout->addWidget(resetButton);
    
    controlLayout->addStretch();
    
    mainLayout->addWidget(controlGroupBox);

    QGroupBox *hintGroupBox = new QGroupBox("Hint", this);
    QVBoxLayout *hintLayout = new QVBoxLayout(hintGroupBox);

    statusLabel = new QLabel("You are black. Beat the min-max AI!", this);
    statusLabel->setStyleSheet("font-weight: 700; color: #1f5f49; background: transparent;");
    hintLayout->addWidget(statusLabel);

    hintTextDisplay = new QTextEdit(this);
    hintTextDisplay->setReadOnly(true);
    hintTextDisplay->setMinimumHeight(120);
    hintTextDisplay->setPlainText("ヒントボタンを押すと、min-max のおすすめ手がここに表示されます。");
    hintLayout->addWidget(hintTextDisplay);

    mainLayout->addWidget(hintGroupBox);

    QGroupBox *rankingGroupBox = new QGroupBox("Ranking", this);
    QVBoxLayout *rankingLayout = new QVBoxLayout(rankingGroupBox);

    rankingTextDisplay = new QTextEdit(this);
    rankingTextDisplay->setReadOnly(true);
    rankingTextDisplay->setMinimumHeight(120);
    rankingLayout->addWidget(rankingTextDisplay);

    mainLayout->addWidget(rankingGroupBox);
    updateRankingDisplay();
    
    setCentralWidget(centralWidget);
    resize(720, 960);
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
            hintTextDisplay->setPlainText(QString("ヒントを作れませんでした: %1").arg(error.what()));
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
        hintTextDisplay->setPlainText(hint.text);
        statusLabel->setText(QString("Hint: Row %1, Column %2").arg(hintRow + 1).arg(hintCol + 1));
        
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
        handleGameOver(state);
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
            makeRandomPlayerMove();
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

void MainWindow::makeRandomPlayerMove()
{
    GameState state = gameManager->getGameState();
    if (state.gameOver || state.validMoves.empty()) {
        gameManager->makeMove(-1, -1);
        return;
    }

    const int index = QRandomGenerator::global()->bounded(static_cast<int>(state.validMoves.size()));
    const auto [row, col] = state.validMoves.at(static_cast<std::size_t>(index));
    statusLabel->setText(QString("Time up! A random move was played: Row %1, Column %2")
                             .arg(row + 1)
                             .arg(col + 1));
    gameManager->makeMove(row, col);
}

void MainWindow::handleGameOver(const GameState &state)
{
    if (rankingRecorded) {
        return;
    }

    rankingRecorded = true;
    const int score = state.blackCount - state.whiteCount;
    const QString result = score >= 0
                               ? QString("Your score: +%1").arg(score)
                               : QString("Your score: %1").arg(score);

    bool ok = false;
    QString name = QInputDialog::getText(
        this,
        "Register Score",
        QString("Game over! %1\nEnter your name:").arg(result),
        QLineEdit::Normal,
        "Player",
        &ok
    ).trimmed();

    if (!ok) {
        name = "Player";
    }
    if (name.isEmpty()) {
        name = "Player";
    }

    saveRanking({
        name,
        score,
        state.blackCount,
        state.whiteCount,
        QDateTime::currentDateTime().toString(Qt::ISODate)
    });
    updateRankingDisplay();
    statusLabel->setText("Score registered. Check the ranking board.");
}

void MainWindow::updateRankingDisplay()
{
    if (!rankingTextDisplay) {
        return;
    }

    QVector<RankingEntry> rankings = loadRankings();
    std::stable_sort(rankings.begin(), rankings.end(), [](const RankingEntry &lhs, const RankingEntry &rhs) {
        return lhs.score > rhs.score;
    });

    QString text;
    text += "Rank  Name              Score   Stones\n";
    text += "--------------------------------------\n";

    const int limit = std::min(10, static_cast<int>(rankings.size()));
    for (int i = 0; i < limit; ++i) {
        const RankingEntry &entry = rankings.at(i);
        const QString scoreText = entry.score >= 0
                                      ? QString("+%1").arg(entry.score)
                                      : QString::number(entry.score);
        text += QString("%1    %2 %3   B%4-W%5\n")
                    .arg(i + 1, 2)
                    .arg(entry.name.left(16).leftJustified(16, ' '))
                    .arg(scoreText.rightJustified(5, ' '))
                    .arg(entry.blackCount)
                    .arg(entry.whiteCount);
    }

    if (rankings.empty()) {
        text += "No scores yet. Finish a game to register your name.\n";
    }

    rankingTextDisplay->setPlainText(text);
}

void MainWindow::saveRanking(const RankingEntry &entry)
{
    QVector<RankingEntry> rankings = loadRankings();
    rankings.push_back(entry);

    std::stable_sort(rankings.begin(), rankings.end(), [](const RankingEntry &lhs, const RankingEntry &rhs) {
        return lhs.score > rhs.score;
    });

    QJsonArray array;
    for (const RankingEntry &ranking : rankings) {
        QJsonObject object;
        object["name"] = ranking.name;
        object["score"] = ranking.score;
        object["blackCount"] = ranking.blackCount;
        object["whiteCount"] = ranking.whiteCount;
        object["playedAt"] = ranking.playedAt;
        array.append(object);
    }

    const QString path = rankingFilePath();
    QDir().mkpath(QFileInfo(path).absolutePath());

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        statusLabel->setText("Could not save ranking.");
        return;
    }

    file.write(QJsonDocument(array).toJson(QJsonDocument::Indented));
}

QVector<MainWindow::RankingEntry> MainWindow::loadRankings() const
{
    QFile file(rankingFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (!document.isArray()) {
        return {};
    }

    QVector<RankingEntry> rankings;
    const QJsonArray array = document.array();
    rankings.reserve(array.size());
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        rankings.push_back({
            object.value("name").toString("Player"),
            object.value("score").toInt(),
            object.value("blackCount").toInt(),
            object.value("whiteCount").toInt(),
            object.value("playedAt").toString()
        });
    }
    return rankings;
}

QString MainWindow::rankingFilePath() const
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (basePath.isEmpty()) {
        basePath = QDir::homePath() + "/.reversi_gui";
    }
    return QDir(basePath).filePath("rankings.json");
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
    
    // Update avatar based on the same evaluator used by the min-max engine.
    if (state.gameOver) {
        if (state.blackCount > state.whiteCount) {
            avatarLabel->setText("🏆"); // Black wins
        } else if (state.whiteCount > state.blackCount) {
            avatarLabel->setText("🏆"); // White wins
        } else {
            avatarLabel->setText("🤝"); // Draw
        }
    } else {
        const int evaluation = hintEngine ? hintEngine->evaluatePosition(state.board, 1) : 0;
        if (evaluation > 60) {
            avatarLabel->setText("😄"); // Black is winning
        } else if (evaluation < -60) {
            avatarLabel->setText("😢"); // Black is losing
        } else {
            avatarLabel->setText("😊"); // Balanced
        }
    }
}
