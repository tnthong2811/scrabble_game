#pragma once
#include "BaseStrategy.h"
#include "../Utils/MonteCarloSimulator.h"

namespace AI {
namespace Strategies {

class ExpertStrategy : public BaseStrategy {
public:
    ExpertStrategy(const Dictionary& dictionary);
    std::vector<Play> generatePlays(const Board& board, const std::vector<Tile>& rack) override;

private:
    MonteCarloSimulator simulator_;
};

} // namespace Strategies
} // namespace AI