#pragma once
#include "Strategies/BaseStrategy.h"
#include "Heuristics/Heuristic.h"
#include "core/Play.h"        
#include "core/Board.h"      
#include "core/Tile.h"        
#include "core/dictionary/trie_dictionary.hpp" 
#include <memory>

namespace AI {

enum class Difficulty {
    EASY,     
    MEDIUM,    
    HARD,      
};

class ScrabbleAI {
public:
    ScrabbleAI(Difficulty difficulty, const TrieDictionary& dictionary);
    ~ScrabbleAI();
    std::vector<Play> generateTopPlays(const Board& board, const std::vector<Tile>& rack, int topN);
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