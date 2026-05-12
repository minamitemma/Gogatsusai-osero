#ifndef AIENGINE_H
#define AIENGINE_H

#include <vector>
#include <utility>

/**
 * @brief AIエンジンの抽象インターフェース
 * 
 * 共同開発者が実装する6×6 オセロAIエンジンは、
 * このインターフェースを実装する必要があります。
 */
class IAIEngine
{
public:
    virtual ~IAIEngine() = default;
    
    /**
     * AIから次の手を取得
     * @param board ゲーム盤面 (0: empty, 1: black, 2: white)
     * @param player 現在のプレイヤー (1: black, 2: white)
     * @return 推奨される手 (row, col)
     */
    virtual std::pair<int, int> getNextMove(
        const std::vector<std::vector<int>> &board,
        int player
    ) = 0;
    
    /**
     * AIエンジンの初期化
     */
    virtual void initialize() = 0;
    
    /**
     * AIエンジンをシャットダウン
     */
    virtual void shutdown() = 0;
};

#endif // AIENGINE_H
