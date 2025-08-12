#pragma once
#include "core/Board.h"
#include "core/Tile.h"
#include "core/Move.h" 

namespace AI {
namespace Strategies {

class BaseStrategy {
public:
    virtual ~BaseStrategy() = default;
    virtual std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) = 0;
};

} // namespace Strategies
} // namespace AI