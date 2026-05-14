#include "boardwidget.h"
#include <QPainter>
#include <QMouseEvent>

BoardWidget::BoardWidget(QWidget *parent)
    : QWidget(parent)
{
    // Initialize 6x6 board with starting position
    board = std::vector<std::vector<int>>(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    // Starting position for 6x6 Othello
    board[2][2] = 1; // Black
    board[2][3] = 2; // White
    board[3][2] = 2; // White
    board[3][3] = 1; // Black
}

void BoardWidget::updateBoard(const std::vector<std::vector<int>> &newBoard)
{
    board = newBoard;
    update(); // Trigger repaint
}

void BoardWidget::setValidMoves(const std::vector<std::pair<int, int>> &moves)
{
    validMoves = moves;
    update(); // Trigger repaint
}

void BoardWidget::setHintMove(const std::pair<int, int> &move)
{
    hintMove = move;
    update();
}

void BoardWidget::clearHintMove()
{
    hintMove.reset();
    update();
}

void BoardWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    int cellSize = getCellSize();
    int boardPixelSize = cellSize * BOARD_SIZE;
    int xOffset = (width() - boardPixelSize) / 2;
    int yOffset = (height() - boardPixelSize) / 2;
    
    // Draw board background
    painter.fillRect(rect(), QColor("#f6efe4"));
    painter.setPen(QPen(QColor("#1c4d3b"), 4));
    painter.setBrush(QColor("#2f9f6b"));
    painter.drawRoundedRect(xOffset, yOffset, boardPixelSize, boardPixelSize, 10, 10);
    
    // Draw grid lines
    painter.setPen(QPen(QColor("#174735"), 2));
    for (int i = 0; i <= BOARD_SIZE; ++i) {
        int pos = i * cellSize;
        painter.drawLine(xOffset + pos, yOffset, xOffset + pos, yOffset + boardPixelSize);
        painter.drawLine(xOffset, yOffset + pos, xOffset + boardPixelSize, yOffset + pos);
    }
    
    // Draw stones
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int x = xOffset + col * cellSize + cellSize / 2;
            int y = yOffset + row * cellSize + cellSize / 2;
            int radius = cellSize / 2 - 5;
            
            if (board[row][col] == 1) {
                // Black stone
                painter.setPen(QPen(QColor("#111111"), 2));
                painter.setBrush(QColor("#202020"));
                painter.drawEllipse(x - radius, y - radius, radius * 2, radius * 2);
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(255, 255, 255, 45));
                painter.drawEllipse(x - radius / 2, y - radius / 2, radius / 2, radius / 2);
            } else if (board[row][col] == 2) {
                // White stone
                painter.setPen(QPen(QColor("#d8d2c6"), 2));
                painter.setBrush(QColor("#fffdf5"));
                painter.drawEllipse(x - radius, y - radius, radius * 2, radius * 2);
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(80, 80, 80, 30));
                painter.drawEllipse(x - radius / 3, y - radius / 3, radius / 2, radius / 2);
            }
        }
    }
    
    // Draw valid move indicators
    painter.setPen(QPen(QColor("#9c6a00"), 2));
    painter.setBrush(QColor(255, 203, 69, 150));
    for (const auto &move : validMoves) {
        int x = xOffset + move.second * cellSize + cellSize / 2;
        int y = yOffset + move.first * cellSize + cellSize / 2;
        painter.drawEllipse(x - 8, y - 8, 16, 16);
    }

    if (hintMove.has_value()) {
        const auto [row, col] = hintMove.value();
        if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
            const int x = xOffset + col * cellSize;
            const int y = yOffset + row * cellSize;
            painter.setPen(QPen(QColor("#ffcb45"), 5));
            painter.setBrush(QColor(255, 203, 69, 70));
            painter.drawRoundedRect(x + 4, y + 4, cellSize - 8, cellSize - 8, 8, 8);
        }
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    auto [row, col] = getClickedCell(event->pos().x(), event->pos().y());
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
        emit cellClicked(row, col);
    }
}

int BoardWidget::getCellSize() const
{
    return std::min(width(), height()) / BOARD_SIZE;
}

std::pair<int, int> BoardWidget::getClickedCell(int x, int y) const
{
    int cellSize = getCellSize();
    int boardPixelSize = cellSize * BOARD_SIZE;
    int xOffset = (width() - boardPixelSize) / 2;
    int yOffset = (height() - boardPixelSize) / 2;
    
    int col = (x - xOffset) / cellSize;
    int row = (y - yOffset) / cellSize;
    
    return {row, col};
}
