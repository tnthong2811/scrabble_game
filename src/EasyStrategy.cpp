#include "AI/Strategies/EasyStrategy.h"
#include <random>

namespace AI {
namespace Strategies {

EasyStrategy::EasyStrategy(const TrieDictionary& dictionary) : dictionary_(dictionary) {}  // New

std::vector<Move> EasyStrategy::generatePlays(const Board& board, const std::vector<Tile>& rack) {
    std::vector<Move> plays;
    
    const int MAX_ATTEMPTS = 50;
    std::random_device rd;
    std::mt19937 gen(rd()); 
    
    for (int i = 0; i < MAX_ATTEMPTS; i++) {
        if (plays.size() >= 5) break; 

        Move play = generateRandomValidPlay(board, rack, gen);
        if (play.isValid() && dictionary_.contains(play.getWord())) {  // Check dict
            plays.push_back(play);
        }
    }
    
    return plays;
}

Move EasyStrategy::generateRandomValidPlay(const Board& board, const std::vector<Tile>& rack, std::mt19937& gen) {
    std::uniform_int_distribution<> dirDist(0, 1);

    std::string word;
    for (const Tile& tile : rack) {
        if (!tile.isBlank()) {
            word += tile.getLetter();
        }
        if (word.length() >= 3) break;
    }
    if (word.empty()) {
        for (const Tile& tile : rack) {
            if (tile.isBlank()) {
                word = "A"; 
                break;
            }
        }
    }
    if (word.empty()) return Move("", 0, 0, Move::Direction::HORIZONTAL); 

    // Use anchor for position
    auto anchors = findAnchorPoints(board);  // Add function similar to Hard
    if (anchors.empty()) return Move("", 0, 0, Move::Direction::HORIZONTAL); 
    auto anchor = anchors[gen() % anchors.size()];
    int row = anchor.first;
    int col = anchor.second;

    Move::Direction dir = (dirDist(gen) == 0) ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL;

    return Move(word, row, col, dir);
}

// Add this function
std::vector<std::pair<int, int>> EasyStrategy::findAnchorPoints(const Board& board) {
    std::vector<std::pair<int, int>> anchors;
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            if (board.isAnchor(i, j)) {
                anchors.emplace_back(i, j);
            }
        }
    }
    if (anchors.empty()) anchors.emplace_back(7, 7);  // Center if empty
    return anchors;
}

} // namespace Strategies
} // namespace AI