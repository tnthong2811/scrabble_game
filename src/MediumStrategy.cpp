#include "AI/Strategies/MediumStrategy.h"
#include <algorithm>
#include <set>

namespace AI {
namespace Strategies {

MediumStrategy::MediumStrategy(const TrieDictionary& dictionary) : dictionary_(dictionary) {}

std::vector<Move> MediumStrategy::generatePlays(const Board& board, const std::vector<Tile>& rack) {
    std::set<Move> potential_plays;
    
    std::unordered_map<char, int> rackLetters;
    int blankCount = 0;
    for (const auto& tile : rack) {
        if (tile.isBlank()) blankCount++;
        else rackLetters[tile.getLetter()]++;
    }

    if (board.isEmpty()) {
        auto words = dictionary_.find_possible_words_with_blank(rackToString(rack), blankCount);
        for (const auto& word : words) {
            if (word.length() > 1) {
                potential_plays.insert(Move(word, 7, 7, Move::Direction::HORIZONTAL));
            }
        }
        return std::vector<Move>(potential_plays.begin(), potential_plays.end());
    }

    for (int r = 0; r < 15; ++r) {
        for (int c = 0; c < 15; ++c) {
            if (board.isAnchor(r, c)) {
                generateMovesForAnchor(board, r, c, rackLetters, blankCount, potential_plays);
            }
        }
    }

    return std::vector<Move>(potential_plays.begin(), potential_plays.end());
}

void MediumStrategy::generateMovesForAnchor(const Board& board, int r, int c, 
                                            const std::unordered_map<char, int>& rackLetters, int blankCount,
                                            std::set<Move>& plays) {
    std::string prefix = "";
    int limit = 0;
    for (int i = c - 1; i >= 0 && board.hasTile(r, i); --i) {
        // *** SỬA LỖI: Dùng getCell(r, i).tile thay vì getTile(r, i) ***
        prefix = board.getCell(r, i).tile.getLetter() + prefix;
        limit++;
    }
    if(prefix.empty() || dictionary_.isPrefix(prefix)){
        extendRight(board, prefix, r, c, rackLetters, blankCount, limit, plays);
    }

    prefix = "";
    limit = 0;
    for (int i = r - 1; i >= 0 && board.hasTile(i, c); --i) {
        // *** SỬA LỖI: Dùng getCell(i, c).tile thay vì getTile(i, c) ***
        prefix = board.getCell(i, c).tile.getLetter() + prefix;
        limit++;
    }
    if(prefix.empty() || dictionary_.isPrefix(prefix)){
        extendDown(board, prefix, r, c, rackLetters, blankCount, limit, plays);
    }
}

void MediumStrategy::extendRight(const Board& board, std::string currentWord, int r, int c,
                                 std::unordered_map<char, int> rackLetters, int blankCount, int limit,
                                 std::set<Move>& plays) {
    if (c < 15 && board.hasTile(r, c)) {
        // *** SỬA LỖI: Dùng getCell(r, c).tile thay vì getTile(r, c) ***
        char letter = board.getCell(r, c).tile.getLetter();
        if (dictionary_.isPrefix(currentWord + letter)) {
            extendRight(board, currentWord + letter, r, c + 1, rackLetters, blankCount, limit, plays);
        }
        return;
    }

    if (dictionary_.contains(currentWord) && limit > 0) {
        plays.insert(Move(currentWord, r, c - currentWord.length(), Move::Direction::HORIZONTAL));
    }
    if (c >= 15) return;

    for (auto const& [letter, count] : rackLetters) {
        if (count > 0) {
            if (dictionary_.isPrefix(currentWord + letter)) {
                auto newRack = rackLetters;
                newRack[letter]--;
                extendRight(board, currentWord + letter, r, c + 1, newRack, blankCount, limit + 1, plays);
            }
        }
    }
    if (blankCount > 0) {
        for (char l = 'A'; l <= 'Z'; ++l) {
            if (dictionary_.isPrefix(currentWord + l)) {
                extendRight(board, currentWord + l, r, c + 1, rackLetters, blankCount - 1, limit + 1, plays);
            }
        }
    }
}

void MediumStrategy::extendDown(const Board& board, std::string currentWord, int r, int c,
                                std::unordered_map<char, int> rackLetters, int blankCount, int limit,
                                std::set<Move>& plays) {
    if (r < 15 && board.hasTile(r, c)) {
        // *** SỬA LỖI: Dùng getCell(r, c).tile thay vì getTile(r, c) ***
        char letter = board.getCell(r, c).tile.getLetter();
        if (dictionary_.isPrefix(currentWord + letter)) {
            extendDown(board, currentWord + letter, r + 1, c, rackLetters, blankCount, limit, plays);
        }
        return;
    }
    if (dictionary_.contains(currentWord) && limit > 0) {
        plays.insert(Move(currentWord, r - currentWord.length(), c, Move::Direction::VERTICAL));
    }
    if (r >= 15) return;

    for (auto const& [letter, count] : rackLetters) {
        if (count > 0) {
            if (dictionary_.isPrefix(currentWord + letter)) {
                auto newRack = rackLetters;
                newRack[letter]--;
                extendDown(board, currentWord + letter, r + 1, c, newRack, blankCount, limit + 1, plays);
            }
        }
    }
    if (blankCount > 0) {
        for (char l = 'A'; l <= 'Z'; ++l) {
            if (dictionary_.isPrefix(currentWord + l)) {
                extendDown(board, currentWord + l, r + 1, c, rackLetters, blankCount - 1, limit + 1, plays);
            }
        }
    }
}

std::string MediumStrategy::rackToString(const std::vector<Tile>& rack) {
    std::string s = "";
    for (const auto& tile : rack) {
        if (!tile.isBlank()) s += tile.getLetter();
    }
    return s;
}

} // namespace Strategies
} // namespace AI
