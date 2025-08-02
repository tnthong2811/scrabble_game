#include "AI/Heuristics/Heuristic.h"
#include "core/Player.h"
#include <utility> // Cho std::pair

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

    // 1. Bingo bonus (dùng hết rack)
    if (move.getTilesUsed().size() == Player::MAX_RACK_SIZE) {
        score += 50.0f;
    }

    // 2. Premium square utilization
    // Giả định Move::getCoveredPositions() trả về vector<pair<int, int>>
    for (const auto& pos : move.getCoveredPositions()) {
        int row = pos.first;
        int col = pos.second;
        const auto& cell = board.getCell(row, col); // Giả định Board có getCell(row, col)
        if (!cell.isPremiumUsed) { // Giả định isPremiumUsed là phương thức
            switch (cell.type) { // Giả định getType() thay vì type
                case Board::CellType::DOUBLE_LETTER: score += 0.5f; break;
                case Board::CellType::TRIPLE_LETTER: score += 1.0f; break;
                case Board::CellType::DOUBLE_WORD: score += 1.5f; break;
                case Board::CellType::TRIPLE_WORD: score += 2.5f; break;
                default: break;
            }
        }
    }

    // 3. Rack balance after play
    int vowelCount = 0;
    for (const Tile& tile : remainingRack) {
        if (tile.isVowel()) vowelCount++;
    }
    score -= static_cast<float>(vowelCount) * 0.5f;

    // 4. Risk exposure
    // Giả định getCoveredPositions() thay vì getAdjacentPositions()
    for (const auto& pos : move.getCoveredPositions()) {
        int row = pos.first;
        int col = pos.second;
        const auto& cell = board.getCell(row, col);
        if (!cell.hasTile()) { // Giả định hasTile() kiểm tra ô trống
            auto cellType = cell.type;
            if (cellType == Board::CellType::DOUBLE_WORD || cellType == Board::CellType::TRIPLE_WORD) {
                score -= 2.0f * (cellType == Board::CellType::TRIPLE_WORD ? 1.5f : 1.0f) * riskFactor_;
            }
        }
    }

    // 5. Tile conservation (giữ lại tile tốt)
    for (const Tile& tile : remainingRack) {
        if (tile.getValue() >= 5) {
            score += 0.3f;
        }
    }

    return score * aggressiveness_;
}

} // namespace Heuristics
} // namespace AI