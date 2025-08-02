#include "AI/Strategies/MediumStrategy.h"
#include "AI/Heuristics/Heuristic.h"
#include <algorithm>
#include <random>
#include <functional>

namespace AI {
namespace Strategies {

std::vector<Move> MediumStrategy::generatePlays(const Board& board, const std::vector<Tile>& rack) {
    std::vector<Move> plays;
    AI::Heuristics::BasicHeuristic heuristic;

    // 1. Tìm tất cả vị trí có thể đặt từ
    std::vector<std::pair<int, int>> validPositions = findValidPositions(board);
    
    // 2. Tạo các từ ngắn (2-4 chữ) để tối ưu thời gian
    const int MAX_WORD_LENGTH = 4;
    std::vector<std::string> potentialWords = generatePotentialWords(rack, MAX_WORD_LENGTH);
    
    // 3. Thử đặt các từ vào các vị trí hợp lệ
    for (const auto& pos : validPositions) {
        for (bool horizontal : {true, false}) {
            for (const auto& word : potentialWords) {
                Play play = tryPlaceWord(board, pos, horizontal, word, rack);
                Move move = play.getMove(); // Giả định Play có getMove()
                if (move.isValid()) {   // Kiểm tra tính hợp lệ của Move
                    int score = calculateScore(play, board);
                    move.setScore(score); // Gán điểm cho Move
                    plays.push_back(move);
                }
            }
        }
    }
    
    // 4. Sắp xếp và giới hạn số lượng plays
    std::sort(plays.begin(), plays.end(), [&](const Move& a, const Move& b) {
        return heuristic.evaluate(a, board, {}) > heuristic.evaluate(b, board, {});
    });

    if (plays.size() > 20) {
        plays.erase(plays.begin() + 20, plays.end());
    }
    
    return plays;
}

std::vector<std::pair<int, int>> MediumStrategy::findValidPositions(const Board& board) {
    std::vector<std::pair<int, int>> positions;
    
    // Ưu tiên các ô gần từ đã có
    for (int row = 0; row < Board::SIZE; ++row) {
        for (int col = 0; col < Board::SIZE; ++col) {
            if (!board.getCell(row, col).hasTile() && 
                board.isAdjacentToTile(row, col)) { // Sử dụng isAdjacentToTile công khai
                positions.emplace_back(row, col);
            }
        }
    }
    
    // Nếu là lượt đầu, chọn ô trung tâm
    if (positions.empty() && board.isEmpty()) {
        positions.emplace_back(Board::SIZE/2, Board::SIZE/2);
    }
    
    return positions;
}

std::vector<std::string> MediumStrategy::generatePotentialWords(const std::vector<Tile>& rack, int maxLength) {
    std::vector<char> letters;
    for (const auto& tile : rack) {
        letters.push_back(tile.getLetter());
    }
    
    std::vector<std::string> words;
    std::string current;
    
    std::function<void(size_t, int)> generate = [&](size_t start, int length) { // Sử dụng size_t cho start
        if (length > 0 && length <= maxLength) {
            words.push_back(current);
        }
        
        if (length == maxLength) return;
        
        for (size_t i = start; i < letters.size(); ++i) { // Sử dụng size_t cho i
            current.push_back(letters[i]);
            generate(i + 1, length + 1);
            current.pop_back();
        }
    };
    
    generate(0, 0);
    return words;
}

Play MediumStrategy::tryPlaceWord(const Board& board, 
                                const std::pair<int, int>& pos, 
                                bool horizontal,
                                const std::string& word,
                                const std::vector<Tile>& rack) {
    Move move(word, pos.first, pos.second, 
              horizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL);
    if (board.canPlaceWord(move)) {
        return Play(move);
    }
    return Play(Move("", 0, 0, Move::Direction::HORIZONTAL)); // Trả về Move không hợp lệ
}

int MediumStrategy::calculateScore(const Play& play, const Board& board) {
    return board.calculateScore(play.getMove());
}

} // namespace Strategies
} // namespace AI