#include "AI/Utils/GADDAG.h"
#include <stack>
#include <queue>
#include <algorithm>
#include <functional>
#include <string> 
namespace AI {
namespace Utils {

GADDAG::GADDAG() : root_(std::make_shared<Node>()) {}

void GADDAG::buildFromDictionary(const TrieDictionary& dictionary) {
    for (const std::string& word : dictionary.getAllWords()) {
        if (word.length() >= 2) { 
            addWord(word);
        }
    }
}

void GADDAG::addWord(const std::string& word) {
    std::string reversed(word.rbegin(), word.rend());
    
    for (size_t i = 1; i <= word.length(); ++i) {
        std::string transformed = reversed.substr(0, i) + ">" + word.substr(word.length() - i);
        insertWord(transformed);
    }
}

void GADDAG::insertWord(const std::string& word) {
    auto current = root_;
    
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = std::make_shared<Node>();
        }
        current = current->children[c];
    }
    
    current->isTerminal = true;
}

bool GADDAG::isWordValid(const std::string& word) const {
    std::string reversed(word.rbegin(), word.rend());
    
    for (size_t i = 1; i <= word.length(); ++i) {
        std::string query = reversed.substr(0, i) + ">" + word.substr(word.length() - i);
        if (searchWord(query)) {
            return true;
        }
    }
    
    return false;
}

bool GADDAG::searchWord(const std::string& word) const {
    auto current = root_;
    
    for (char c : word) {
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            return false;
        }
        current = it->second;
    }
    
    return current->isTerminal;
}

std::vector<Move> GADDAG::generatePlaysForPosition(const Board& board,
                                                 const BoardPosition& pos,
                                                 bool horizontal,
                                                 const std::vector<Tile>& rack) const {
    std::vector<Move> plays;
    
    std::vector<char> rackLetters;
    std::vector<bool> isBlank(rack.size(), false);
    for (size_t i = 0; i < rack.size(); ++i) {
        rackLetters.push_back(rack[i].getLetter());
        if (rack[i].isBlank()) isBlank[i] = true;
    }
    
    std::vector<std::string> validWords = findValidWords(pos, horizontal, board, rackLetters, isBlank);
    
    for (const auto& word : validWords) {
        Move move(word, pos.row, pos.col, horizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL);
        move.setScore(calculateScore(word, board, pos, horizontal));
        
        std::vector<Tile> tilesUsed;
        std::vector<bool> used(rack.size(), false);
        for (size_t i = 0; i < word.length() && i < rack.size(); ++i) {
            for (size_t j = 0; j < rack.size(); ++j) {
                if (!used[j] && (rack[j].getLetter() == word[i] || (isBlank[j] && word[i] >= 'A' && word[i] <= 'Z'))) {
                    tilesUsed.push_back(rack[j]);
                    used[j] = true;
                    break;
                }
            }
        }
        move.addTileUsed(tilesUsed); 
        plays.push_back(move);
    }
    
    return plays;
}

std::vector<std::string> GADDAG::findValidWords(const BoardPosition& pos,
                                              bool horizontal,
                                              const Board& board,
                                              const std::vector<char>& rackLetters,
                                              const std::vector<bool>& isBlank) const {
    std::vector<std::string> validWords;
    
    std::string prefix, suffix;
    BoardPosition current = pos;
    
    if (horizontal) {
        while (current.col > 0 && board.getTileLetter(current.row, current.col - 1) != ' ') {
            current.col--;
            prefix = board.getTileLetter(current.row, current.col) + prefix;
        }
    } else {
        while (current.row > 0 && board.getTileLetter(current.row - 1, current.col) != ' ') {
            current.row--;
            prefix = board.getTileLetter(current.row, current.col) + prefix;
        }
    }
    
    current = pos;
    if (horizontal) {
        while (current.col < Board::SIZE - 1 && board.getTileLetter(current.row, current.col + 1) != ' ') {
            current.col++;
            suffix += board.getTileLetter(current.row, current.col);
        }
    } else {
        while (current.row < Board::SIZE - 1 && board.getTileLetter(current.row + 1, current.col) != ' ') {
            current.row++;
            suffix += board.getTileLetter(current.row, current.col);
        }
    }
    
    std::string pattern = prefix + "#" + suffix;
    std::vector<std::string> candidates = findWordsWithPattern(pattern, rackLetters, isBlank);
    
    for (const auto& word : candidates) {
        if (word.find(prefix) == 0 && 
            word.length() >= prefix.length() + suffix.length() &&
            word.substr(word.length() - suffix.length()) == suffix) {
            validWords.push_back(word);
        }
    }
    
    return validWords;
}

std::vector<std::string> GADDAG::findWordsWithPattern(const std::string& pattern,
                                                     const std::vector<char>& rackLetters,
                                                     const std::vector<bool>& isBlank) const {
    std::vector<std::string> results;
    int blankCount = std::count(isBlank.begin(), isBlank.end(), true);
    std::unordered_map<char, int> letterCount;  // New: Count for duplicate
    for (char c : rackLetters) letterCount[c]++;

    auto search = [this, &results, &letterCount, &blankCount, &rackLetters, &isBlank, &pattern](auto&& self, 
                                                                                       std::shared_ptr<Node> node, 
                                                                                       std::string current, 
                                                                                       std::string::size_type patternPos, 
                                                                                       bool reversed) -> void {
        if (patternPos >= pattern.length()) {
            if (node->isTerminal) {
                if (reversed) {
                    std::reverse(current.begin(), current.end());
                }
                results.push_back(current);
            }
            auto it = node->children.find('>');
            if (it != node->children.end()) {
                self(self, it->second, current, patternPos, true);
            }
            return;
        }

        char expected = pattern[patternPos];
        if (expected == '#') {
            for (const auto& child : node->children) {
                char c = child.first;
                if (c == '>') continue;

                if (letterCount[c] > 0) {  
                    letterCount[c]--;
                    self(self, child.second, current + c, patternPos + 1, reversed);
                    letterCount[c]++;
                } else if (blankCount > 0) {
                    blankCount--;
                    self(self, child.second, current + c, patternPos + 1, reversed);
                    blankCount++;
                }
            }
        } else {
            auto it = node->children.find(expected);
            if (it != node->children.end()) {
                self(self, it->second, current + expected, patternPos + 1, reversed);
            }
        }
    };

    search(search, root_, "", 0, false);
    return results;
}

int GADDAG::calculateScore(const std::string& word, const Board& board, const BoardPosition& pos, bool horizontal) const {
    int score = 0;
    int multiplier = 1;
    for (size_t i = 0; i < word.length(); ++i) {
        int r = pos.row + (horizontal ? 0 : i);
        int c = pos.col + (horizontal ? i : 0);
        if (board.isValidPosition(r, c)) {
            int letterScore = Tile(word[i], false).getValue();
            auto cellType = board.getCell(r, c).type;
            if (cellType == CellType::DOUBLE_LETTER) letterScore *= 2;
            else if (cellType == CellType::TRIPLE_LETTER) letterScore *= 3;
            else if (cellType == CellType::DOUBLE_WORD) multiplier *= 2;
            else if (cellType == CellType::TRIPLE_WORD) multiplier *= 3;
            score += letterScore;
        }
    }
    return score * multiplier;
}

} // namespace Utils
} // namespace AI