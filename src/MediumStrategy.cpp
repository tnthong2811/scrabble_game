#include "AI/Strategies/MediumStrategy.h"
#include <algorithm>
#include <functional>

namespace AI {
namespace Strategies {

MediumStrategy::MediumStrategy(const TrieDictionary& dictionary) : dictionary_(dictionary) {}    
// Hàm chính được đơn giản hóa: chỉ tạo ra các nước đi, không xác thực.
std::vector<Move> MediumStrategy::generatePlays(const Board& board, const std::vector<Tile>& rack) {
    std::vector<Move> potential_plays;

    // 1. Tìm các vị trí có thể bắt đầu một nước đi
    std::vector<std::pair<int, int>> valid_positions = findValidPositions(board);

    // 2. Tạo các từ có thể có từ khay chữ (giới hạn độ dài cho cấp độ Medium)
    const int MAX_WORD_LENGTH = 7;  // Increase to 7
    std::vector<std::string> valid_words = generatePotentialWords(rack, MAX_WORD_LENGTH);
    
    // 3. Kết hợp từ và vị trí để tạo ra danh sách các nước đi (Move)
    for (const auto& pos : valid_positions) {
        for (bool is_horizontal : {true, false}) {
            for (const auto& word : valid_words) {
                // AI chỉ cần "đề xuất" nước đi. Lớp Game sẽ kiểm tra xem nó có hợp lệ không.
                potential_plays.emplace_back(word, pos.first, pos.second,
                                             is_horizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL);
            }
        }
    }

    // AI không cần sắp xếp hay giới hạn số lượng ở đây nữa,
    // vì ScrabbleAI sẽ đánh giá tất cả các nước đi này.
    return potential_plays;
}

// Hàm này tìm các ô trống nằm cạnh một chữ cái đã có.
std::vector<std::pair<int, int>> MediumStrategy::findValidPositions(const Board& board) {
    std::vector<std::pair<int, int>> positions;

    // Nếu bàn cờ trống, vị trí duy nhất có thể đi là ô trung tâm.
    if (board.isEmpty()) {
        positions.emplace_back(Board::SIZE / 2, Board::SIZE / 2);
        return positions;
    }

    // Nếu không, tìm tất cả các "điểm neo".
    for (int row = 0; row < Board::SIZE; ++row) {
        for (int col = 0; col < Board::SIZE; ++col) {
            // isAnchor đã được public và AI có thể gọi nó.
            if (board.isAnchor(row, col)) {
                positions.emplace_back(row, col);
            }
        }
    }
    return positions;
}

// Hàm này tạo ra các từ có thể có từ các chữ cái trên tay.
std::vector<std::string> MediumStrategy::generatePotentialWords(const std::vector<Tile>& rack, int maxLength) {
    std::string letters;
    int blankCount = 0;
    for (const auto& tile : rack) {
        if (tile.isBlank()) {
            blankCount++;
        } else {
            letters += tile.getLetter();
        }
    }
    // Use new dict func for generate with blank
    auto potential = dictionary_.find_possible_words_with_blank(letters, blankCount);
    // Filter unique and sort (optional)
    std::sort(potential.begin(), potential.end());
    potential.erase(std::unique(potential.begin(), potential.end()), potential.end());
    return potential;
}

} // namespace Strategies
} // namespace AI