#pragma once
#include "BaseStrategy.h"
#include "core/Board.h"
#include "AI/Utils/GADDAG.h"
#include "core/dictionary/trie_dictionary.hpp"
#include "core/Board.h"
#include <vector>
#include <string>
#include <utility>
#include <unordered_set>

namespace AI {
namespace Strategies {

class HardStrategy : public BaseStrategy {
public:
    HardStrategy(const TrieDictionary& dictionary);
    std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) override;

private:
    const TrieDictionary& dictionary_;
    Utils::GADDAG gaddag_;
    void generateMoves(const Board& board, const std::vector<Tile>& rack, std::vector<Move>& plays);

    void gen(int row, int col, bool horizontal, const Board& board,
             std::unordered_map<char, int>& rackCount, int& blanks,
             std::shared_ptr<Utils::GADDAG::Node> node, std::string& currentWord,
             std::vector<Tile>& used, int& currentScore, int& wordMult,
             std::vector<Move>& plays, int limit);

    void go(int row, int col, bool horizontal, const Board& board,
            std::unordered_map<char, int>& rackCount, int& blanks,
            std::shared_ptr<Utils::GADDAG::Node> node, std::string& currentWord,
            std::vector<Tile>& used, int& currentScore, int& wordMult,
            std::vector<Move>& plays, int limit);
            
    std::unordered_set<char> computeCrossSet(int row, int col, bool perpVertical, const Board& board);

    std::vector<std::pair<int, int>> findAnchorPoints(const Board& board);
};

} // namespace Strategies
} // namespace AI