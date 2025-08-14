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
    // 1. Lấy tất cả các nước đi "tiềm năng" từ chiến lược
    std::vector<Move> potential_moves = strategy_->generatePlays(board, rack);
    
    // 2. Tạo một Player tạm thời để dùng cho việc xác thực
    Player tempPlayer("suggester");
    tempPlayer.setRack(rack);

    // 3. Lọc và xác thực từng nước đi bằng hàm của Board
    std::vector<Play> valid_plays;
    for (const auto& move : potential_moves) {
        // Gọi hàm xác thực CÓ SẴN trong Board.cpp
        MoveResult result = board.validateAndScoreMove(move, tempPlayer, dictionary_);
        
        if (result.isValid) {
            Play p(move);
            p.score = result.score; // Sử dụng điểm số đã được xác thực
            valid_plays.push_back(p);
        }
    }
    
    // 4. Sắp xếp các nước đi ĐÃ HỢP LỆ dựa trên điểm số
    std::sort(valid_plays.begin(), valid_plays.end(), [](const Play& a, const Play& b) {
        return a.score > b.score;
    });
    
    // 5. Cắt danh sách để chỉ giữ lại topN nước đi tốt nhất
    if (valid_plays.size() > static_cast<size_t>(topN)) {
        valid_plays.resize(static_cast<size_t>(topN));
    }
    
    return valid_plays;
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