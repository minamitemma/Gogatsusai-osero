#ifndef SIMPLEHINTENGINE_H
#define SIMPLEHINTENGINE_H

#include "aiengine.h"

/**
 * @brief シンプルなヒント エンジン
 * 
 * 基本的な評価関数に基づいてヒントを提供します。
 * ネット上のAI APIが利用できるようになるまで、
 * ローカルで簡単なアルゴリズムを使用します。
 */
class SimpleHintEngine : public IAIEngine
{
public:
    SimpleHintEngine();
    
    std::pair<int, int> getNextMove(
        const std::vector<std::vector<int>> &board,
        int player
    ) override;
    
    void initialize() override;
    void shutdown() override;

private:
    /**
     * 盤面の評価値を計算
     * より多くの石をとる手ほど高い評価値
     */
    int evaluateMove(
        const std::vector<std::vector<int>> &board,
        int row, int col, int player
    );
    
    int countFlips(
        const std::vector<std::vector<int>> &board,
        int row, int col, int player,
        int dRow, int dCol
    );
};

#endif // SIMPLEHINTENGINE_H
