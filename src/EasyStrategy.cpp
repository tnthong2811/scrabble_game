#include "AI/Strategies/EasyStrategy.h"
#include "core/Board.h"
#include <random>
#include <algorithm>
#include <vector>

namespace AI {
namespace Strategies {

EasyStrategy::EasyStrategy(const TrieDictionary& dictionary) : dictionary_(dictionary) {}

std::vector<Move> EasyStrategy::generatePlays(const Board& board, const std::vector<Tile>& rack) {
    std::vector<Move> potential_plays;
    
    // 1. Lấy tất cả các từ có thể tạo ra từ rack
    std::string rack_letters;
    int blank_count = 0;
    for (const auto& tile : rack) {
        if (tile.isBlank()) {
            blank_count++;
        } else {
            rack_letters += tile.getLetter();
        }
    }
    std::vector<std::string> possible_words = dictionary_.find_possible_words_with_blank(rack_letters, blank_count);

    // 2. Xáo trộn danh sách từ và các điểm neo để tạo sự ngẫu nhiên
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(possible_words.begin(), possible_words.end(), gen);

    auto anchors = findAnchorPoints(board);
    std::shuffle(anchors.begin(), anchors.end(), gen);

    // 3. Thử đặt các từ vào các điểm neo cho đến khi tìm thấy một vài nước đi hợp lệ
    for (const auto& word : possible_words) {
        if (word.length() < 2) continue; // Bỏ qua các từ quá ngắn

        for (const auto& anchor : anchors) {
            for (bool is_horizontal : {true, false}) {
                Move move(word, anchor.first, anchor.second, 
                          is_horizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL);
                
                // Trả về danh sách các nước đi tiềm năng
                potential_plays.push_back(move);
                if (potential_plays.size() >= 10) { // Giới hạn số lượng gợi ý 
                    return potential_plays;
                }
            }
        }
    }
    
    return potential_plays;
}

std::vector<std::pair<int, int>> EasyStrategy::findAnchorPoints(const Board& board) {
    std::vector<std::pair<int, int>> anchors;
    if (board.isEmpty()) {
        anchors.emplace_back(7, 7);
        return anchors;
    }

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
