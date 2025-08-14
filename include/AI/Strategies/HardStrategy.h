#include "AI/Strategies/BaseStrategy.h"
#include "AI/Utils/GADDAG.h"
#include "core/Board.h"
#include "core/Move.h"
#include "core/Tile.h"
#include "core/dictionary/trie_dictionary.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace AI {
namespace Strategies {

class HardStrategy : public BaseStrategy {
private:
    Utils::GADDAG gaddag_;
    const TrieDictionary& dictionary_;

    std::vector<std::pair<int, int>> findAnchorPoints(const Board& board);
    std::string rackToString(const std::vector<Tile>& rack);
    bool crossCheck(int r, int c, char letter, bool isHorizontal, const Board& board);
    std::string getCrossWord(int r, int c, char letter, bool isHorizontal, const Board& board);
    std::vector<char> selectTilesToSwap(const std::unordered_map<char, int>& rackCount, int blanks);

    void gen(int r, int c, bool isHorizontal, const Board& board,
             std::unordered_map<char, int> rackCount, int blanks,
             std::shared_ptr<Utils::Node> node, const std::string& word_part,
             std::unordered_set<Move, Move::Hash>& plays, int anchor_r, int anchor_c);
    
    void go(int r, int c, bool isHorizontal, const Board& board,
            std::unordered_map<char, int> rackCount, int blanks,
            std::shared_ptr<Utils::Node> node, const std::string& word_part,
            std::unordered_set<Move, Move::Hash>& plays, int anchor_r, int anchor_c);

public:
    explicit HardStrategy(const TrieDictionary& dictionary);
    std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) override;
};

} // namespace Strategies
} // namespace AI
