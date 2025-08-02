#pragma once
#include "../../Core/Move.h"
#include "../../Core/Board.h"
#include "../../core/Play.h" 

namespace AI {
namespace Heuristics {

class Heuristic {
public:
    virtual ~Heuristic() = default;
    virtual float evaluate(const Play& play, 
                          const Board& board, 
                          const std::vector<Tile>& remainingRack) = 0;
    
    void setAggressiveness(float value) { aggressiveness_ = value; }
    void setRiskFactor(float value) { riskFactor_ = value; }

protected:
    float aggressiveness_ = 1.0f;  // 0.5-2.0
    float riskFactor_ = 1.0f;      // 0.5-1.5
};

class BasicHeuristic : public Heuristic {
public:
    float evaluate(const Play& play, 
                  const Board& board, 
                  const std::vector<Tile>& remainingRack) override;
};

class AdvancedHeuristic : public Heuristic {
public:
    float evaluate(const Play& play, 
                  const Board& board, 
                  const std::vector<Tile>& remainingRack) override;
};

} // namespace Heuristics
} // namespace AI