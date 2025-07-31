#pragma once
#include "../../Core/Board.h"
#include "../../Core/Tile.h"
#include "../../Core/Move.h" // Thay Play bằng Move

namespace AI {
namespace Strategies {

class BaseStrategy {
public:
    virtual ~BaseStrategy() = default;
    virtual std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) = 0;
};

} // namespace Strategies
} // namespace AI