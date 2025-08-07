#include "AI/ScrabbleAI.h"
#include "AI/Strategies/EasyStrategy.h"
#include "AI/Strategies/MediumStrategy.h"
#include "AI/Strategies/HardStrategy.h"
#include "AI/Heuristics/Heuristic.h"
#include <algorithm>

namespace AI {

ScrabbleAI::ScrabbleAI(Difficulty difficulty, const TrieDictionary& dictionary)
    : difficulty_(difficulty), dictionary_(dictionary) {
    initializeStrategy();
}

ScrabbleAI::~ScrabbleAI() = default;

void ScrabbleAI::initializeStrategy() {
    switch (difficulty_) {
        case Difficulty::EASY:
            strategy_ = std::make_unique<Strategies::EasyStrategy>();
            heuristic_ = std::make_unique<Heuristics::BasicHeuristic>();
            break;
            
        case Difficulty::MEDIUM:
            strategy_ = std::make_unique<Strategies::MediumStrategy>();
            heuristic_ = std::make_unique<Heuristics::AdvancedHeuristic>();
            break;
            
        case Difficulty::HARD:
            strategy_ = std::make_unique<Strategies::HardStrategy>(dictionary_);
            heuristic_ = std::make_unique<Heuristics::AdvancedHeuristic>();
            break;
    }
}

Play ScrabbleAI::generatePlay(const Board& board, const std::vector<Tile>& rack) {
    std::vector<Move> moves = strategy_->generatePlays(board, rack);
    
    if (moves.empty()) {
        return Play::createPass();
    }
    
    Play bestPlay(moves[0]); 
    float bestScore = heuristic_->evaluate(bestPlay, board, calculateRemainingRack(rack, bestPlay));
    
    for (size_t i = 1; i < moves.size(); ++i) {
        Play currentPlay(moves[i]); 
        
        std::vector<Tile> remainingRack = calculateRemainingRack(rack, currentPlay);
        float score = heuristic_->evaluate(currentPlay, board, remainingRack);
        
        if (score > bestScore) {
            bestScore = score;
            bestPlay = currentPlay;
        }
    }
    
    return bestPlay;
}

void ScrabbleAI::setDifficulty(Difficulty newDifficulty) {
    if (difficulty_ != newDifficulty) {
        difficulty_ = newDifficulty;
        initializeStrategy();
    }
}

std::vector<Tile> ScrabbleAI::calculateRemainingRack(const std::vector<Tile>& rack, const Play& play) {
    std::vector<Tile> remaining = rack;
    const auto& tilesUsed = play.getMove().getTilesUsed(); 
    for (const Tile& usedTile : tilesUsed) {
        auto it = std::find_if(remaining.begin(), remaining.end(),
                               [&usedTile](const Tile& t) {
                                   return t.getLetter() == usedTile.getLetter() && t.getValue() == usedTile.getValue() && t.isBlank() == usedTile.isBlank();
                               });
        if (it != remaining.end()) {
            remaining.erase(it);
        }
    }
    return remaining;
}

} // namespace AI