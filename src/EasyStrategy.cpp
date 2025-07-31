#include "AI/Strategies/EasyStrategy.h"
#include <random>

namespace AI {
namespace Strategies {

std::vector<Move> EasyStrategy::generatePlays(const Board& board, const std::vector<Tile>& rack) {
    std::vector<Move> plays;
    
    // Đơn giản: tạo các play ngẫu nhiên hợp lệ
    const int MAX_ATTEMPTS = 50;
    std::random_device rd;
    std::mt19937 gen(rd()); // Khởi tạo generator
    
    for (int i = 0; i < MAX_ATTEMPTS; i++) {
        if (plays.size() >= 5) break; // Giới hạn số lượng plays
        
        // Tạo nước đi ngẫu nhiên đơn giản
        Move play = generateRandomValidPlay(board, rack, gen);
        if (play.isValid()) {
            plays.push_back(play);
        }
    }
    
    return plays;
}

Move EasyStrategy::generateRandomValidPlay(const Board& board, const std::vector<Tile>& rack, std::mt19937& gen) {
    // Logic đơn giản: Chọn từ ngẫu nhiên từ rack và đặt ở vị trí ngẫu nhiên
    std::uniform_int_distribution<> rowDist(0, Board::SIZE - 1);
    std::uniform_int_distribution<> colDist(0, Board::SIZE - 1);
    std::uniform_int_distribution<> dirDist(0, 1);

    // Tạo từ ngẫu nhiên từ rack
    std::string word;
    for (const Tile& tile : rack) {
        if (!tile.isBlank()) {
            word += tile.getLetter();
        }
        if (word.length() >= 3) break; // Giới hạn độ dài từ
    }
    if (word.empty()) {
        // Nếu không có từ, thử sử dụng blank tile nếu có
        for (const Tile& tile : rack) {
            if (tile.isBlank()) {
                word = "A"; // Gán một ký tự ngẫu nhiên
                break;
            }
        }
    }
    if (word.empty()) return Move("", 0, 0, Move::Direction::HORIZONTAL); // Không có từ hợp lệ

    // Chọn vị trí và hướng ngẫu nhiên
    int row = rowDist(gen);
    int col = colDist(gen);
    Move::Direction dir = (dirDist(gen) == 0) ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL;

    return Move(word, row, col, dir);
}

} // namespace Strategies
} // namespace AI