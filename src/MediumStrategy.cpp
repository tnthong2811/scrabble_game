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
    const int MAX_WORD_LENGTH = 5;
    std::vector<std::string> potential_words = generatePotentialWords(rack, MAX_WORD_LENGTH);
    std::vector<std::string> valid_words;
    for (const auto& word : potential_words) {
        if (dictionary_.contains(word)) {
            valid_words.push_back(word);
        }
    }
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
    std::vector<char> letters;
    int blankCount = 0;
    for (const auto& tile : rack) {
        if (tile.isBlank()) {
            blankCount++;
        } else {
            letters.push_back(tile.getLetter());
        }
    }

    std::vector<std::string> words;
    std::sort(letters.begin(), letters.end());

    if (blankCount == 0) {
        do {
            for (int len = 2; len <= std::min((int)letters.size(), maxLength); ++len) {
                words.emplace_back(letters.begin(), letters.begin() + len);
            }
        } while (std::next_permutation(letters.begin(), letters.end()));
    } else {
        // Simple wildcard for 1 blank
        std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (char rep : alphabet) {
            std::vector<char> temp = letters;
            temp.push_back(rep);
            std::sort(temp.begin(), temp.end());
            do {
                for (int len = 2; len <= std::min((int)temp.size(), maxLength); ++len) {
                    words.emplace_back(temp.begin(), temp.begin() + len);
                }
            } while (std::next_permutation(temp.begin(), temp.end()));
        }
    }

    std::sort(words.begin(), words.end());
    words.erase(std::unique(words.begin(), words.end()), words.end());
    return words;
}

} // namespace Strategies
} // namespace AI