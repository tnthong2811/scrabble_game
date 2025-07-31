#pragma once
#include "BaseStrategy.h"
#include "core/Play.h"
#include "core/Board.h"
#include "core/Tile.h"
#include <vector>
#include <string>

namespace AI {
namespace Strategies {

class MediumStrategy : public BaseStrategy {
public:
    std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) override;

private:
    std::vector<std::pair<int, int>> findValidPositions(const Board& board);
    std::vector<std::string> generatePotentialWords(const std::vector<Tile>& rack, int maxLength);
    Play tryPlaceWord(const Board& board, const std::pair<int, int>& pos, bool horizontal, 
                      const std::string& word, const std::vector<Tile>& rack);
    int calculateScore(const Play& play, const Board& board);
};

} // namespace Strategies
} // namespace AI