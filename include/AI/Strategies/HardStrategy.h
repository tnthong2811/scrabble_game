#pragma once
#include "BaseStrategy.h"
#include "AI/Utils/GADDAG.h"
#include "core/Board.h"
#include "core/Tile.h"
#include "core/dictionary/trie_dictionary.hpp"

namespace AI {
namespace Strategies {

class HardStrategy : public BaseStrategy {
public:
    HardStrategy(const TrieDictionary& dictionary);
    std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) override;

private:
    ::AI::Utils::GADDAG gaddag_; // Đối tượng GADDAG
    std::vector<Move> findPlaysWithGADDAG(const Board& board, const std::vector<Tile>& rack);
    static std::vector<std::pair<int, int>> findAnchorPoints(const Board& board);
};

} // namespace Strategies
} // namespace AI