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
    bool isValidPlacement(const Board& board, const Move& move, const std::vector<Tile>& rack);
private:
    const TrieDictionary& dictionary_;
    std::vector<std::pair<int, int>> findAnchorPoints(const Board& board);
};

} // namespace Strategies
} // namespace AI