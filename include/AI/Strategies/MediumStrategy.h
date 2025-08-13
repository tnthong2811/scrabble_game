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
    void generateMovesForAnchor(const Board& board, int r, int c, 
                                const std::unordered_map<char, int>& rackLetters, int blankCount,
                                std::set<Move>& plays);

    void extendRight(const Board& board, std::string currentWord, int r, int c,
                     std::unordered_map<char, int> rackLetters, int blankCount, int limit,
                     std::set<Move>& plays);

    void extendDown(const Board& board, std::string currentWord, int r, int c,
                    std::unordered_map<char, int> rackLetters, int blankCount, int limit,
                    std::set<Move>& plays);

    std::string rackToString(const std::vector<Tile>& rack);
};

} // namespace Strategies
} // namespace AI