#pragma once
#include "BaseStrategy.h"
#include "../../Core/Move.h"
#include <random> // Thêm include cho std::mt19937

namespace AI {
namespace Strategies {

class EasyStrategy : public BaseStrategy {
public:
    std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) override;
private:
    Move generateRandomValidPlay(const Board& board, const std::vector<Tile>& rack, std::mt19937& gen); // Sửa thành std::mt19937&
};

} // namespace Strategies
} // namespace AI