#include "AI/Strategies/HardStrategy.h"
#include <iostream>
#include <algorithm>
#include <random>

namespace AI {
namespace Strategies {

HardStrategy::HardStrategy(const TrieDictionary& dictionary) : dictionary_(dictionary) {
    std::cout << "Initializing Hard Strategy..." << std::endl;
    if (gaddag_.loadFromFile("assets/gaddag.dat")) {
        std::cout << "GADDAG loaded successfully from assets/gaddag.dat." << std::endl;
    } else {
        std::cerr << "WARNING: Could not load gaddag.dat. Building from dictionary..." << std::endl;
        gaddag_.buildFromDictionary(dictionary);
        std::cout << "GADDAG built. Consider running the build_gaddag tool for faster startups." << std::endl;
    }
}

std::vector<Move> HardStrategy::generatePlays(const Board& board, const std::vector<Tile>& rack) {
    std::unordered_set<Move, Move::Hash> plays_set;
    
    std::unordered_map<char, int> rackCount;
    int blanks = 0;
    for (const auto& t : rack) {
        if (t.isBlank()) blanks++;
        else rackCount[t.getLetter()]++;
    }

    // Debug: In ra rack để kiểm tra
    std::cout << "AI rack: ";
    for (const auto& [letter, count] : rackCount) {
        std::cout << letter << "(" << count << ") ";
    }
    std::cout << "Blanks: " << blanks << std::endl;

    if (board.isEmpty()) {
        auto words = dictionary_.find_possible_words_with_blank(rackToString(rack), blanks);
        for (const auto& word : words) {
            if (word.length() > 1) { 
                plays_set.insert(Move(word, 7, 7, Move::Direction::HORIZONTAL));
            }
        }
        return std::vector<Move>(plays_set.begin(), plays_set.end());
    }

    auto anchors = findAnchorPoints(board);

    for (const auto& anchor : anchors) {
        int anchor_r = anchor.first;
        int anchor_c = anchor.second;
        go(anchor_r, anchor_c, true, board, rackCount, blanks, gaddag_.getRoot(), "", plays_set, anchor_r, anchor_c); // Ngang
        go(anchor_r, anchor_c, false, board, rackCount, blanks, gaddag_.getRoot(), "", plays_set, anchor_r, anchor_c); // Dọc
        gen(anchor_r, anchor_c, true, board, rackCount, blanks, gaddag_.getRoot(), "", plays_set, anchor_r, anchor_c); // Ngang
        gen(anchor_r, anchor_c, false, board, rackCount, blanks, gaddag_.getRoot(), "", plays_set, anchor_r, anchor_c); // Dọc
    }

    if (plays_set.empty()) {
        std::vector<char> tiles_to_swap = selectTilesToSwap(rackCount, blanks);
        if (!tiles_to_swap.empty()) {
            std::cout << "AI swaps tiles: ";
            for (char c : tiles_to_swap) std::cout << c << " ";
            std::cout << std::endl;
            return {}; 
        }
    }

    return std::vector<Move>(plays_set.begin(), plays_set.end());
}

// Hàm chọn tiles để swap thông minh
std::vector<char> HardStrategy::selectTilesToSwap(const std::unordered_map<char, int>& rackCount, int blanks) {
    std::vector<char> tiles_to_swap;
    // Ưu tiên swap chữ cái khó: Q, J, Z, K, X, Y
    std::vector<char> hard_tiles = {'Q', 'J', 'Z', 'K', 'X', 'Y'};
    for (char c : hard_tiles) {
        if (rackCount.count(c) && rackCount.at(c) > 0) {
            tiles_to_swap.push_back(c);
            if (tiles_to_swap.size() >= 3) break; // Swap tối đa 3  tiles
        }
    }

    if (tiles_to_swap.size() < 3) {
        std::vector<char> vowels = {'A', 'E', 'I', 'O', 'U'};
        for (const auto& [letter, count] : rackCount) {
            if (count > 1 && std::find(vowels.begin(), vowels.end(), letter) == vowels.end() && tiles_to_swap.size() < 2) {
                tiles_to_swap.push_back(letter);
            }
        }
    }
    return tiles_to_swap;
}

// Hàm "Gen" - xây dựng phần bên TRÁI/TRÊN của từ
void HardStrategy::gen(int r, int c, bool isHorizontal, const Board& board,
                       std::unordered_map<char, int> rackCount, int blanks,
                       std::shared_ptr<Utils::Node> node, const std::string& word_part,
                       std::unordered_set<Move, Move::Hash>& plays, int anchor_r, int anchor_c) {
    
    int prev_r = isHorizontal ? r : r - 1;
    int prev_c = isHorizontal ? c - 1 : c;

    if (!board.isValidPosition(prev_r, prev_c)) return;

    if (board.hasTile(prev_r, prev_c)) {
        char letter = board.getCell(prev_r, prev_c).tile.getLetter();
        auto next_node = gaddag_.followArc(node, letter);
        if (next_node) {
            gen(prev_r, prev_c, isHorizontal, board, rackCount, blanks, next_node, letter + word_part, plays, anchor_r, anchor_c);
        }
    } else { // Ô trống, thử dùng chữ trên tay
        for (auto const& [letter, count] : rackCount) {
            if (count > 0 && crossCheck(prev_r, prev_c, letter, isHorizontal, board)) {
                auto next_node = gaddag_.followArc(node, letter);
                if (next_node) {
                    rackCount[letter]--;
                    gen(prev_r, prev_c, isHorizontal, board, rackCount, blanks, next_node, letter + word_part, plays, anchor_r, anchor_c);
                    rackCount[letter]++;
                }
            }
        }
        if (blanks > 0) {
            for (char l = 'A'; l <= 'Z'; ++l) {
                if (crossCheck(prev_r, prev_c, l, isHorizontal, board)) {
                    auto next_node = gaddag_.followArc(node, l);
                    if (next_node) {
                        gen(prev_r, prev_c, isHorizontal, board, rackCount, blanks - 1, next_node, l + word_part, plays, anchor_r, anchor_c);
                    }
                }
            }
        }
    }
    
    auto branch_node = gaddag_.followArc(node, '>');
    if (branch_node && !word_part.empty()) {
        go(r, c, isHorizontal, board, rackCount, blanks, branch_node, word_part + ">", plays, anchor_r, anchor_c);
    }
}

// Hàm "Go" - xây dựng phần bên PHẢI/DƯỚI của từ (thêm param anchor_r, anchor_c)
void HardStrategy::go(int r, int c, bool isHorizontal, const Board& board,
                      std::unordered_map<char, int> rackCount, int blanks,
                      std::shared_ptr<Utils::Node> node, const std::string& word_part,
                      std::unordered_set<Move, Move::Hash>& plays, int anchor_r, int anchor_c) {
    
    if (!board.isValidPosition(r, c)) return;

    if (!board.hasTile(r, c)) { // Ô hiện tại trống
        if (node->isTerminal && !word_part.empty()) {
            std::string final_word = word_part;
            final_word.erase(std::remove(final_word.begin(), final_word.end(), '>'), final_word.end()); // Xóa '>'
            
            size_t hook_pos = word_part.find('>');
            size_t prefix_len = (hook_pos != std::string::npos) ? hook_pos : 0;
            
            int start_r = anchor_r;
            int start_c = anchor_c;
            if (isHorizontal) {
                start_c -= prefix_len;
            } else {
                start_r -= prefix_len;
            }
            plays.insert(Move(final_word, start_r, start_c, isHorizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL));
        }

        int next_r = isHorizontal ? r : r + 1;
        int next_c = isHorizontal ? c + 1 : c;

        for (auto const& [letter, count] : rackCount) {
            if (count > 0 && crossCheck(r, c, letter, isHorizontal, board)) {
                auto next_node = gaddag_.followArc(node, letter);
                if (next_node) {
                    rackCount[letter]--;
                    go(next_r, next_c, isHorizontal, board, rackCount, blanks, next_node, word_part + letter, plays, anchor_r, anchor_c);
                    rackCount[letter]++;
                }
            }
        }
        if (blanks > 0) {
            for (char l = 'A'; l <= 'Z'; ++l) {
                if(crossCheck(r, c, l, isHorizontal, board)) {
                    auto next_node = gaddag_.followArc(node, l);
                    if (next_node) {
                        go(next_r, next_c, isHorizontal, board, rackCount, blanks - 1, next_node, word_part + l, plays, anchor_r, anchor_c);
                    }
                }
            }
        }
    } else { // Ô hiện tại đã có chữ
        char letter = board.getCell(r, c).tile.getLetter();
        auto next_node = gaddag_.followArc(node, letter);
        if (next_node) {
            int next_r = isHorizontal ? r : r + 1;
            int next_c = isHorizontal ? c + 1 : c;
            go(next_r, next_c, isHorizontal, board, rackCount, blanks, next_node, word_part + letter, plays, anchor_r, anchor_c);
        }
    }
}

bool HardStrategy::crossCheck(int r, int c, char letter, bool isHorizontal, const Board& board) {
    std::string crossWord = getCrossWord(r, c, letter, isHorizontal, board);
    return crossWord.length() <= 1 || dictionary_.contains(crossWord);
}

std::string HardStrategy::getCrossWord(int r, int c, char letter, bool isHorizontal, const Board& board) {
    if (isHorizontal) { 
        std::string prefix, suffix;
        for (int i = r - 1; i >= 0 && board.hasTile(i, c); --i) prefix = board.getCell(i, c).tile.getLetter() + prefix;
        for (int i = r + 1; i < 15 && board.hasTile(i, c); ++i) suffix += board.getCell(i, c).tile.getLetter();
        return prefix + letter + suffix;
    } else {
        std::string prefix, suffix;
        for (int i = c - 1; i >= 0 && board.hasTile(r, i); --i) prefix = board.getCell(r, i).tile.getLetter() + prefix;
        for (int i = c + 1; i < 15 && board.hasTile(r, i); ++i) suffix += board.getCell(r, i).tile.getLetter();
        return prefix + letter + suffix;
    }
}

std::vector<std::pair<int, int>> HardStrategy::findAnchorPoints(const Board& board) {
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

std::string HardStrategy::rackToString(const std::vector<Tile>& rack) {
    std::string s;
    for(const auto& t : rack) if(!t.isBlank()) s += t.getLetter();
    return s;
}

} // namespace Strategies
} // namespace AI