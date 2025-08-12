#pragma once
#include "BaseStrategy.h"
#include "core/Board.h"
#include "core/Move.h"
#include "core/Tile.h"
#include "core/dictionary/trie_dictionary.hpp"
#include <vector>
#include <string>
#include <utility> 

namespace AI {
namespace Strategies {

class MediumStrategy : public BaseStrategy {
public:
    MediumStrategy(const TrieDictionary& dictionary);
    std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) override;

private:
    const TrieDictionary& dictionary_;

    std::vector<std::pair<int, int>> findValidPositions(const Board& board);
    std::vector<std::string> generatePotentialWords(const std::vector<Tile>& rack, int maxLength);
};

} // namespace Strategies
} // namespace AI