#pragma once
#include "../../Core/Board.h"
#include "../../Core/TileBag.h"
#include <vector>

namespace AI {
namespace Utils {

struct SimulationResult {
    int aiScore = 0;
    int opponentScore = 0;
    bool aiWon = false;
};

class MonteCarloSimulator {
public:
    MonteCarloSimulator(const Dictionary& dictionary);
    
    SimulationResult simulatePlay(const Play& play,
                                const Board& board,
                                const std::vector<Tile>& aiRack,
                                const std::vector<Tile>& opponentRack,
                                const TileBag& tileBag);
    
    void setSimulationCount(int count) { simulationCount_ = count; }

private:
    const Dictionary& dictionary_;
    int simulationCount_ = 100;
    
    Board cloneAndApplyPlay(const Board& board, const Play& play);
    std::vector<Tile> drawTiles(TileBag& bag, int count);
};

} // namespace Utils
} // namespace AI