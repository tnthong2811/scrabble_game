#include "AI/Heuristics/Heuristic.h"
#include "core/Player.h"
#include <utility> 

namespace AI {
namespace Heuristics {

float BasicHeuristic::evaluate(const Play& play, 
                             const Board& board, 
                             const std::vector<Tile>& remainingRack) {
    const Move& move = play.getMove();
    float score = static_cast<float>(move.getScore());
    return score * aggressiveness_;
}

float AdvancedHeuristic::evaluate(const Play& play, 
                                const Board& board, 
                                const std::vector<Tile>& remainingRack) {
    const Move& move = play.getMove();
    float score = static_cast<float>(move.getScore());

    if (move.getTilesUsed().size() == Player::MAX_RACK_SIZE) {
        score += 50.0f;
    }

    for (const auto& pos : move.getCoveredPositions()) {
        int row = pos.first;
        int col = pos.second;
        const auto& cell = board.getCell(row, col);
        if (!cell.isPremiumUsed) { 
            switch (cell.type) { 
                case Board::CellType::DOUBLE_LETTER: score += 0.5f; break;
                case Board::CellType::TRIPLE_LETTER: score += 1.0f; break;
                case Board::CellType::DOUBLE_WORD: score += 1.5f; break;
                case Board::CellType::TRIPLE_WORD: score += 2.5f; break;
                default: break;
            }
        }
    }

    int vowelCount = 0;
    for (const Tile& tile : remainingRack) {
        if (tile.isVowel()) vowelCount++;
    }
    score -= static_cast<float>(vowelCount) * 0.5f;

    for (const auto& pos : move.getCoveredPositions()) {
        int row = pos.first;
        int col = pos.second;
        const auto& cell = board.getCell(row, col);
        if (!cell.hasTile()) { 
            auto cellType = cell.type;
            if (cellType == Board::CellType::DOUBLE_WORD || cellType == Board::CellType::TRIPLE_WORD) {
                score -= 2.0f * (cellType == Board::CellType::TRIPLE_WORD ? 1.5f : 1.0f) * riskFactor_;
            }
        }
    }

    for (const Tile& tile : remainingRack) {
        if (tile.getValue() >= 5) {
            score += 0.3f;
        }
    }

    return score * aggressiveness_;
}

} // namespace Heuristics
} // namespace AI