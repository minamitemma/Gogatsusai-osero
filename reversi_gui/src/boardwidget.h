#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include <optional>
#include <vector>

class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    BoardWidget(QWidget *parent = nullptr);
    
    void updateBoard(const std::vector<std::vector<int>> &board);
    void setValidMoves(const std::vector<std::pair<int, int>> &moves);
    void setHintMove(const std::pair<int, int> &move);
    void clearHintMove();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void cellClicked(int row, int col);

private:
    int getCellSize() const;
    std::pair<int, int> getClickedCell(int x, int y) const;
    
    static constexpr int BOARD_SIZE = 6;
    std::vector<std::vector<int>> board;
    std::vector<std::pair<int, int>> validMoves;
    std::optional<std::pair<int, int>> hintMove;
    
    // 0: empty, 1: black, 2: white
};

#endif // BOARDWIDGET_H
