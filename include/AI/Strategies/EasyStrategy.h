#pragma once
#include "BaseStrategy.h"
#include "core/Move.h"
#include "core/dictionary/trie_dictionary.hpp" 
#include <random>
#include <vector>
#include <utility> 

namespace AI {
namespace Strategies {

class EasyStrategy : public BaseStrategy {
public:
    EasyStrategy(const TrieDictionary& dictionary);
    std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) override;
private:
    const TrieDictionary& dictionary_;
    Move generateRandomValidPlay(const Board& board, const std::vector<Tile>& rack, std::mt19937& gen);
    std::vector<std::pair<int, int>> findAnchorPoints(const Board& board);
};

} // namespace Strategies
} // namespace AI