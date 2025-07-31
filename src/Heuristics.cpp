#include "AI/Heuristics/Heuristic.h"
#include "core/Player.h" // Thêm include Player.h

namespace AI {
namespace Heuristics {

float AdvancedHeuristic::evaluate(const Play& play, 
                                 const Board& board, 
                                 const std::vector<Tile>& remainingRack) {
    const Move& move = play.getMove(); // Truy cập Move từ Play
    float score = static_cast<float>(move.getScore()); // Chuyển đổi int thành float
    
    // 1. Bingo bonus (dùng hết rack)
    if (move.getTilesUsed().size() == Player::MAX_RACK_SIZE) {
        score += 50.0f;
    }
    
    // 2. Premium square utilization
    for (const auto& pos : move.getCoveredPositions()) {
        const auto& cell = board.getCell(pos.first, pos.second); // Sửa cách truy cập pos
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
    
    // 3. Rack balance after play
    int vowelCount = 0;
    for (const Tile& tile : remainingRack) {
        if (tile.isVowel()) vowelCount++;
    }
    score -= vowelCount * 0.5f;
    
    // 4. Risk exposure
    // Giả định getAdjacentPositions() cần được thêm vào Move
    for (const auto& pos : move.getCoveredPositions()) { // Thay adjacentPositions bằng coveredPositions tạm thời
        const auto& cell = board.getCell(pos.first, pos.second);
        if (!cell.hasTile() && (cell.type == Board::CellType::DOUBLE_WORD || cell.type == Board::CellType::TRIPLE_WORD)) {
            score -= 2.0f * (cell.type == Board::CellType::TRIPLE_WORD ? 1.5f : 1.0f) * riskFactor_;
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