#include "AI/Strategies/HardStrategy.h"
#include <algorithm> // Dùng cho std::sort

namespace AI {
namespace Strategies {

HardStrategy::HardStrategy(const TrieDictionary& dictionary) {
    gaddag_.buildFromDictionary(dictionary); // Khởi tạo GADDAG từ từ điển
}

std::vector<Move> HardStrategy::generatePlays(const Board& board, const std::vector<Tile>& rack) {
    return findPlaysWithGADDAG(board, rack);
}

std::vector<Move> HardStrategy::findPlaysWithGADDAG(const Board& board, const std::vector<Tile>& rack) {
    std::vector<Move> plays;

    auto anchors = findAnchorPoints(board);

    for (const auto& anchor : anchors) {
        ::AI::Utils::GADDAG::BoardPosition pos{anchor.first, anchor.second}; // Tạo BoardPosition
        for (bool horizontal : {true, false}) {
            auto newPlays = gaddag_.generatePlaysForPosition(board, pos, horizontal, rack);
            plays.insert(plays.end(), newPlays.begin(), newPlays.end());
        }
    }

    // Sắp xếp nước đi theo điểm số giảm dần
    std::sort(plays.begin(), plays.end(), [](const Move& a, const Move& b) {
        return a.getScore() > b.getScore();
    });

    // Giới hạn tối đa 20 nước đi
    if (plays.size() > 20) {
        plays.erase(plays.begin() + 20, plays.end());
    }

    return plays;
}

std::vector<std::pair<int, int>> HardStrategy::findAnchorPoints(const Board& board) {
    std::vector<std::pair<int, int>> anchors;
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            if (board.isAnchor(i, j)) {
                anchors.emplace_back(i, j);
            }
        }
    }
    return anchors;
}

} // namespace Strategies
} // namespace AI