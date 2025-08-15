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
    
    // 1. Lấy tất cả các từ có thể tạo ra từ rack, ưu tiên từ ngắn để giữ mức dễ
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

    // Lọc từ ngắn (2-5 chữ cái) để giữ mức dễ, tránh từ dài phức tạp
    std::vector<std::string> short_words;
    for (const auto& word : possible_words) {
        if (word.length() >= 2 && word.length() <= 5) {
            short_words.push_back(word);
        }
    }
    possible_words = short_words;

    // 2. Xáo trộn danh sách từ và các điểm neo để tạo sự ngẫu nhiên
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(possible_words.begin(), possible_words.end(), gen);

    auto anchors = findAnchorPoints(board);
    std::shuffle(anchors.begin(), anchors.end(), gen);

    // 3. Thử đặt các từ vào các điểm neo cho đến khi tìm thấy một vài nước đi hợp lệ
    for (const auto& word : possible_words) {
        for (const auto& anchor : anchors) {
            for (bool is_horizontal : {true, false}) {
                Move move(word, anchor.first, anchor.second, 
                          is_horizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL);
                
                // Kiểm tra tính hợp lệ cơ bản: có thể đặt từ lên board mà không xung đột
                if (isValidPlacement(board, move, rack)) {
                    potential_plays.push_back(move);
                    if (potential_plays.size() >= 5) { // Giới hạn số lượng gợi ý nhỏ để giữ dễ
                        return potential_plays;
                    }
                }
            }
        }
    }
    
    return potential_plays;
}

bool EasyStrategy::isValidPlacement(const Board& board, const Move& move, const std::vector<Tile>& rack) {
    int row = move.getRow();
    int col = move.getCol();
    const std::string& word = move.getWord();
    bool is_horizontal = (move.getDirection() == Move::Direction::HORIZONTAL);

    // Kiểm tra giới hạn bàn cờ
    if (is_horizontal) {
        if (col + word.length() > Board::SIZE) return false;
    } else {
        if (row + word.length() > Board::SIZE) return false;
    }

    // Kiểm tra xem có chỗ trống và khớp với tile hiện có
    for (size_t i = 0; i < word.length(); ++i) {
        int r = row + (is_horizontal ? 0 : i);
        int c = col + (is_horizontal ? i : 0);
        if (board.hasTile(r, c)) {
            if (board.getCell(r, c).tile.getLetter() != word[i]) {
                return false; // Xung đột với tile hiện có
            }
        }
    }

    // Kiểm tra xem rack có đủ chữ cái (bao gồm blank)
    std::string rack_letters;
    int blank_count = 0;
    for (const auto& tile : rack) {
        if (tile.isBlank()) blank_count++;
        else rack_letters += tile.getLetter();
    }

    std::string word_copy = word;
    for (char& ch : word_copy) {
        size_t pos = rack_letters.find(ch);
        if (pos != std::string::npos) {
            rack_letters.erase(pos, 1);
        } else if (blank_count > 0) {
            blank_count--;
        } else {
            return false; // Không đủ chữ cái
        }
    }

    return true;
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