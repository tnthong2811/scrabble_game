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
    std::vector<std::pair<int, int>> validPositions = findValidPositions(board);
    const int MAX_WORD_LENGTH = 4;
    std::vector<std::string> potentialWords = generatePotentialWords(rack, MAX_WORD_LENGTH);
    
    for (const auto& pos : validPositions) {
        for (bool horizontal : {true, false}) {
            for (const auto& word : potentialWords) {
                Play play = tryPlaceWord(board, pos, horizontal, word, rack);
                Move move = play.getMove(); 
                if (move.isValid()) {   
                    int score = calculateScore(play, board);
                    move.setScore(score); 
                    plays.push_back(move);
                }
            }
        }
    }
    
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
    
    for (int row = 0; row < Board::SIZE; ++row) {
        for (int col = 0; col < Board::SIZE; ++col) {
            if (!board.getCell(row, col).hasTile() && 
                board.isAdjacentToTile(row, col)) {
                positions.emplace_back(row, col);
            }
        }
    }
    
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
    
    std::function<void(size_t, int)> generate = [&](size_t start, int length) { 
        if (length > 0 && length <= maxLength) {
            words.push_back(current);
        }
        
        if (length == maxLength) return;
        
        for (size_t i = start; i < letters.size(); ++i) { 
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
    return Play(Move("", 0, 0, Move::Direction::HORIZONTAL));
}

int MediumStrategy::calculateScore(const Play& play, const Board& board) {
    return board.calculateScore(play.getMove());
}

} // namespace Strategies
} // namespace AI