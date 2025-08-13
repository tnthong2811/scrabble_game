#include "AI/ScrabbleAI.h"
#include "AI/Strategies/EasyStrategy.h"
#include "AI/Strategies/MediumStrategy.h"
#include "AI/Strategies/HardStrategy.h"
#include "AI/Heuristics/Heuristic.h"
#include <algorithm>
namespace AI {
ScrabbleAI::ScrabbleAI(Difficulty difficulty, const TrieDictionary& dictionary)
    : difficulty_(difficulty), dictionary_(dictionary) {
    initializeStrategy();
}
ScrabbleAI::~ScrabbleAI() = default;
void ScrabbleAI::initializeStrategy() {
    switch (difficulty_) {
        case Difficulty::EASY:
            strategy_ = std::make_unique<Strategies::EasyStrategy>(dictionary_);
            heuristic_ = std::make_unique<Heuristics::BasicHeuristic>();
            break;
           
        case Difficulty::MEDIUM:
            strategy_ = std::make_unique<Strategies::MediumStrategy>(dictionary_);
            heuristic_ = std::make_unique<Heuristics::AdvancedHeuristic>();
            break;
           
        case Difficulty::HARD:
            strategy_ = std::make_unique<Strategies::HardStrategy>(dictionary_);
            heuristic_ = std::make_unique<Heuristics::AdvancedHeuristic>();
            break;
    }
}
std::vector<Play> ScrabbleAI::generateTopPlays(const Board& board, const std::vector<Tile>& rack, int topN) {
    // 1. Lấy tất cả các nước đi có thể từ chiến lược
    std::vector<Move> moves = strategy_->generatePlays(board, rack);
   
    // 2. Chuyển đổi Move thành Play và chấm điểm heuristic
    std::vector<Play> plays;
    for (auto& m : moves) {
        Play p(m);
        // Tính điểm heuristic và lưu trực tiếp vào đối tượng Play
        p.score = heuristic_->evaluate(p, board, calculateRemainingRack(rack, p));
        plays.push_back(p);
    }
   
    // 3. Sắp xếp danh sách các Play dựa trên điểm số giảm dần
    std::sort(plays.begin(), plays.end(), [](const Play& a, const Play& b) {
        return a.score > b.score;
    });
   
    // 4. Cắt danh sách để chỉ giữ lại topN nước đi tốt nhất
    if (plays.size() > static_cast<size_t>(topN)) {
        plays.resize(static_cast<size_t>(topN));
    }
   
    return plays;
}
void ScrabbleAI::setDifficulty(Difficulty newDifficulty) {
    if (difficulty_ != newDifficulty) {
        difficulty_ = newDifficulty;
        initializeStrategy();
    }
}
std::vector<Tile> ScrabbleAI::calculateRemainingRack(const std::vector<Tile>& rack, const Play& play) {
    std::vector<Tile> remaining = rack;
    const auto& tilesUsed = play.getMove().getTilesUsed();
    for (const Tile& usedTile : tilesUsed) {
        auto it = std::find_if(remaining.begin(), remaining.end(),
                               [&usedTile](const Tile& t) {
                                   if (usedTile.isBlank()) return t.isBlank(); // Flexible for blank
                                   return t.getLetter() == usedTile.getLetter() && t.getValue() == usedTile.getValue() && t.isBlank() == usedTile.isBlank();
                               });
        if (it != remaining.end()) {
            remaining.erase(it);
        }
    }
    return remaining;
}
} // namespace AI