#pragma once
#include "Strategies/BaseStrategy.h"
#include "Heuristics/Heuristic.h"
#include "core/Play.h"        // Thêm include Play.h
#include "core/Board.h"       // Thêm include Board.h
#include "core/Tile.h"        // Thêm include Tile.h
#include "core/dictionary/trie_dictionary.hpp" // Thêm include TrieDictionary.h
#include <memory>

namespace AI {

enum class Difficulty {
    EASY,      // Sử dụng chiến lược ngẫu nhiên
    MEDIUM,    // Heuristic đơn giản
    HARD,      // GADDAG + heuristic nâng cao
    EXPERT     // Mô phỏng Monte Carlo
};

class ScrabbleAI {
public:
    ScrabbleAI(Difficulty difficulty, const TrieDictionary& dictionary);
    ~ScrabbleAI();
    
    Play generatePlay(const Board& board, const std::vector<Tile>& rack);
    void setDifficulty(Difficulty newDifficulty);
    
private:
    Difficulty difficulty_;
    const TrieDictionary& dictionary_;
    std::unique_ptr<AI::Strategies::BaseStrategy> strategy_;
    std::unique_ptr<AI::Heuristics::Heuristic> heuristic_;
    
    void initializeStrategy();
    static std::vector<Tile> calculateRemainingRack(const std::vector<Tile>& rack, const Play& play);
};

} // namespace AI