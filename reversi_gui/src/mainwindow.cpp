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
#include <QSizePolicy>
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
      gameManager(nullptr),
      hintEngine(nullptr),
      hintsRemaining(3),
      humanPlayer(1),
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
    
    startNewGame();
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
    mainLayout->setContentsMargins(22, 18, 22, 22);
    mainLayout->setSpacing(12);

    centralWidget->setStyleSheet(
        "QWidget { background: #f3efe6; color: #26322d; font-size: 15px; }"
        "QGroupBox { background: #fffdf7; border: 1px solid #d2c8b7; border-radius: 8px; "
        "margin-top: 12px; padding: 14px; font-weight: 800; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 7px; color: #1f5f49; }"
        "QPushButton { background: #f6c64c; border: 1px solid #b77a18; border-radius: 8px; "
        "padding: 10px 16px; font-weight: 800; color: #26322d; }"
        "QPushButton:hover { background: #ffd973; }"
        "QPushButton:pressed { background: #e8b43c; }"
        "QPushButton:disabled { background: #d8d2c6; border-color: #b9b0a2; color: #746d64; }"
        "QComboBox { background: #ffffff; border: 1px solid #b8c9bd; border-radius: 8px; "
        "padding: 8px 12px; font-weight: 800; }"
        "QTextEdit { background: #ffffff; border: 1px solid #c9d6ce; border-radius: 8px; "
        "padding: 12px; font-size: 15px; }"
        "QLCDNumber { background: #26322d; color: #ffcb45; border: 3px solid #ffcb45; border-radius: 8px; }"
    );

    QLabel *titleLabel = new QLabel("6x6 Reversi Challenge", this);
    titleLabel->setAlignment(Qt::AlignLeft);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 900; color: #1f5f49; background: transparent;");
    mainLayout->addWidget(titleLabel);
    
    // Top: Game Info Section
    QGroupBox *infoGroupBox = new QGroupBox("Game Information", this);
    QHBoxLayout *infoLayout = new QHBoxLayout(infoGroupBox);
    infoLayout->setSpacing(18);
    
    // Player turn
    currentPlayerLabel = new QLabel("Current Player: Black", this);
    currentPlayerLabel->setStyleSheet("font-size: 20px; font-weight: 900; background: transparent; color: #1b4b39;");
    infoLayout->addWidget(currentPlayerLabel);
    
    // Stone counts
    blackCountLabel = new QLabel("Black: 2", this);
    whiteCountLabel = new QLabel("White: 2", this);
    blackCountLabel->setStyleSheet("background: transparent; font-weight: 800;");
    whiteCountLabel->setStyleSheet("background: transparent; font-weight: 800;");
    infoLayout->addWidget(blackCountLabel);
    infoLayout->addWidget(whiteCountLabel);
    infoLayout->addStretch();
    
    // Timer
    timerDisplay = new QLCDNumber(2, this);
    timerDisplay->setSegmentStyle(QLCDNumber::Flat);
    timerDisplay->setMinimumSize(120, 72);
    timerDisplay->display("10");
    QLabel *timeLabel = new QLabel("Time", this);
    timeLabel->setStyleSheet("font-size: 20px; font-weight: 800; background: transparent;");
    infoLayout->addWidget(timeLabel);
    infoLayout->addWidget(timerDisplay);
    
    mainLayout->addWidget(infoGroupBox);

    QHBoxLayout *playLayout = new QHBoxLayout();
    playLayout->setSpacing(16);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(12);
    
    // Middle: Board Display
    boardWidget = new BoardWidget(this);
    boardWidget->setMinimumSize(440, 440);
    boardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    leftLayout->addWidget(boardWidget, 1);
    
    // Bottom: Control Section
    QGroupBox *controlGroupBox = new QGroupBox("Controls", this);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroupBox);
    controlLayout->setSpacing(10);
    
    // Avatar
    avatarLabel = new QLabel(this);
    avatarLabel->setText("😊");
    avatarLabel->setStyleSheet("font-size: 48px; background: transparent;");
    avatarLabel->setFixedSize(60, 60);
    avatarLabel->setAlignment(Qt::AlignCenter);
    controlLayout->addWidget(avatarLabel);

    QLabel *sideLabel = new QLabel("Play as", this);
    sideLabel->setStyleSheet("font-weight: 800; background: transparent;");
    controlLayout->addWidget(sideLabel);

    sideComboBox = new QComboBox(this);
    sideComboBox->addItem("Black (First)", 1);
    sideComboBox->addItem("White (Second)", 2);
    connect(sideComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        humanPlayer = sideComboBox->currentData().toInt();
        startNewGame();
    });
    controlLayout->addWidget(sideComboBox);
    
    // Hint Button
    hintButton = new QPushButton("Hint (3/3)", this);
    controlLayout->addWidget(hintButton);
    
    // Reset Button
    resetButton = new QPushButton("New Game", this);
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        startNewGame();
    });
    controlLayout->addWidget(resetButton);
    
    controlLayout->addStretch();
    
    leftLayout->addWidget(controlGroupBox);
    playLayout->addLayout(leftLayout, 3);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(12);

    QGroupBox *hintGroupBox = new QGroupBox("Hint", this);
    QVBoxLayout *hintLayout = new QVBoxLayout(hintGroupBox);
    hintLayout->setSpacing(10);

    statusLabel = new QLabel("You are black. Beat the min-max AI!", this);
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("font-weight: 900; color: #1f5f49; background: transparent; font-size: 16px;");
    hintLayout->addWidget(statusLabel);

    hintTextDisplay = new QTextEdit(this);
    hintTextDisplay->setReadOnly(true);
    hintTextDisplay->setMinimumHeight(190);
    hintTextDisplay->setHtml(
        "<div style='font-size: 16px; color: #4b5a51;'>"
        "ヒントボタンを押すと、min-max のおすすめ手・評価値・候補手ランキングがここに表示されます。"
        "</div>"
    );
    hintLayout->addWidget(hintTextDisplay);

    rightLayout->addWidget(hintGroupBox, 2);

    QGroupBox *rankingGroupBox = new QGroupBox("Ranking", this);
    QVBoxLayout *rankingLayout = new QVBoxLayout(rankingGroupBox);

    rankingTextDisplay = new QTextEdit(this);
    rankingTextDisplay->setReadOnly(true);
    rankingTextDisplay->setMinimumHeight(170);
    rankingLayout->addWidget(rankingTextDisplay);

    rightLayout->addWidget(rankingGroupBox, 1);
    playLayout->addLayout(rightLayout, 2);

    mainLayout->addLayout(playLayout, 1);
    updateRankingDisplay();
    
    setCentralWidget(centralWidget);
    resize(1060, 780);
}

void MainWindow::startNewGame()
{
    hintsRemaining = 3;
    aiMovePending = false;
    rankingRecorded = false;
    hintButton->setText(QString("Hint (%1/3)").arg(hintsRemaining));
    hintTextDisplay->setHtml(
        "<div style='font-size: 16px; color: #4b5a51;'>"
        "ヒントボタンを押すと、min-max のおすすめ手・評価値・候補手ランキングがここに表示されます。"
        "</div>"
    );
    statusLabel->setText(QString("New game started. You are %1.").arg(playerSideName()));
    stopTimer();
    resetTimer();

    if (gameManager) {
        gameManager->initializeGame();
    }
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
            hintTextDisplay->setHtml(
                QString("<div style='color: #b9442f; font-weight: 800;'>ヒントを作れませんでした</div>"
                        "<div>%1</div>").arg(QString(error.what()).toHtmlEscaped())
            );
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
        hintTextDisplay->setHtml(hint.text);
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
    return state.currentPlayer != humanPlayer;
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
    const int playerCount = humanPlayer == 1 ? state.blackCount : state.whiteCount;
    const int aiCount = humanPlayer == 1 ? state.whiteCount : state.blackCount;
    const int score = playerCount - aiCount;
    const QString result = score >= 0
                               ? QString("Your score: +%1").arg(score)
                               : QString("Your score: %1").arg(score);

    bool ok = false;
    QString name = QInputDialog::getText(
        this,
        "Register Score",
        QString("Game over! You were %1. %2\nEnter your name:")
            .arg(playerSideName())
            .arg(result),
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
        humanPlayer,
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
    text += "----------------------------------------\n";

    const int limit = std::min(10, static_cast<int>(rankings.size()));
    for (int i = 0; i < limit; ++i) {
        const RankingEntry &entry = rankings.at(i);
        const QString scoreText = entry.score >= 0
                                      ? QString("+%1").arg(entry.score)
                                      : QString::number(entry.score);
        const int playerCount = entry.playerSide == 2 ? entry.whiteCount : entry.blackCount;
        const int aiCount = entry.playerSide == 2 ? entry.blackCount : entry.whiteCount;
        text += QString("%1    %2 %3   P%4-A%5\n")
                    .arg(i + 1, 2)
                    .arg(entry.name.left(16).leftJustified(16, ' '))
                    .arg(scoreText.rightJustified(5, ' '))
                    .arg(playerCount)
                    .arg(aiCount);
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
        object["playerSide"] = ranking.playerSide;
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
            object.value("playerSide").toInt(1),
            object.value("playedAt").toString()
        });
    }
    return rankings;
}

QString MainWindow::playerSideName() const
{
    return humanPlayer == 1 ? "Black" : "White";
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
        const QString sideName = state.currentPlayer == 1 ? "Black" : "White";
        playerText = isAiTurn(state)
                         ? QString("AI thinking: %1").arg(sideName)
                         : QString("Your turn: %1").arg(sideName);
    }
    currentPlayerLabel->setText(playerText);
    
    // Update stone counts
    blackCountLabel->setText(QString("Black: %1").arg(state.blackCount));
    whiteCountLabel->setText(QString("White: %1").arg(state.whiteCount));
    
    // Update avatar based on the same evaluator used by the min-max engine.
    if (state.gameOver) {
        const int playerCount = humanPlayer == 1 ? state.blackCount : state.whiteCount;
        const int aiCount = humanPlayer == 1 ? state.whiteCount : state.blackCount;
        if (playerCount > aiCount) {
            avatarLabel->setText("🏆"); // Player wins
        } else if (aiCount > playerCount) {
            avatarLabel->setText("😢"); // AI wins
        } else {
            avatarLabel->setText("🤝"); // Draw
        }
    } else {
        const int evaluation = hintEngine ? hintEngine->evaluatePosition(state.board, humanPlayer) : 0;
        if (evaluation > 60) {
            avatarLabel->setText("😄"); // Black is winning
        } else if (evaluation < -60) {
            avatarLabel->setText("😢"); // Black is losing
        } else {
            avatarLabel->setText("😊"); // Balanced
        }
    }
}
