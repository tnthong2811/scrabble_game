// HardStrategy.cpp - Use Utils::Node, followArc on node (shared_ptr)
#include "AI/Strategies/HardStrategy.h"
#include <algorithm>
#include <random>
#include <unordered_set>
namespace AI {
namespace Strategies {
HardStrategy::HardStrategy(const TrieDictionary& dictionary) : dictionary_(dictionary) {
    gaddag_.buildFromDictionary(dictionary);
}
// Hàm public chính, gọi đến hàm helper generateMoves
std::vector<Move> HardStrategy::generatePlays(const Board& board, const std::vector<Tile>& rack) {
    std::vector<Move> plays;
    generateMoves(board, rack, plays);
    return plays;
}
// === TRIỂN KHAI CÁC HÀM HELPER MỚI ===
void HardStrategy::generateMoves(const Board& board, const std::vector<Tile>& rack, std::vector<Move>& plays) {
    auto anchors = findAnchorPoints(board);
   
    // Tối ưu: Nếu có quá nhiều điểm neo, chỉ chọn một phần ngẫu nhiên
    if (anchors.size() > 25) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(anchors.begin(), anchors.end(), g);
        anchors.resize(25);
    }
    std::unordered_map<char, int> rackCount;
    int blanks = 0;
    for (const auto& t : rack) {
        if (t.isBlank()) blanks++;
        else rackCount[t.getLetter()]++;
    }
    for (const auto& anchor : anchors) {
        int row = anchor.first;
        int col = anchor.second;
       
        std::string currentWord = "";
        std::vector<Tile> used;
        int currentScore = 0;
        int wordMult = 1;
        // Bắt đầu quá trình đệ quy từ nút gốc của GADDAG
        gen(row, col, true, board, rackCount, blanks, gaddag_.getRoot(), currentWord, used, currentScore, wordMult, plays, 15); // Ngang
        gen(row, col, false, board, rackCount, blanks, gaddag_.getRoot(), currentWord, used, currentScore, wordMult, plays, 15); // Dọc
    }
}
// Hàm "Gen" - đi sang trái từ điểm neo
void HardStrategy::gen(int row, int col, bool horizontal, const Board& board,
                       std::unordered_map<char, int>& rackCount, int& blanks,
                       std::shared_ptr<Utils::Node> node, std::string& currentWord,
                       std::vector<Tile>& used, int& currentScore, int& wordMult,
                       std::vector<Move>& plays, int limit) {
    if (!board.isValidPosition(row, col) || limit <= 0) return;
    // Trường hợp 1: Ô đã có chữ
    if (board.hasTile(row, col)) {
        char letter = board.getTileLetter(row, col);
        auto next_node = gaddag_.followArc(node, letter);
        if (next_node) {
            currentWord = letter + currentWord; // Prepend vì đi trái
            int next_r = horizontal ? row : row - 1;
            int next_c = horizontal ? col - 1 : col;
            gen(next_r, next_c, horizontal, board, rackCount, blanks, next_node, currentWord, used, currentScore, wordMult, plays, limit - 1);
           
            auto branch_node = gaddag_.followArc(next_node, '>');
            if (branch_node) {
                go(row, col, horizontal, board, rackCount, blanks, branch_node, currentWord, used, currentScore, wordMult, plays, 15);
            }
            currentWord.erase(0, 1); // Backtrack xóa front
        }
    }
    // Trường hợp 2: Ô trống
    else {
        auto crossSet = computeCrossSet(row, col, !horizontal, board); // perpVertical = !horizontal
        for (char c = 'A'; c <= 'Z'; ++c) {
            if (crossSet.count(c)) { // Chỉ thử các chữ cái hợp lệ
                bool useBlank = false;
                if (rackCount[c] > 0) {
                    rackCount[c]--;
                } else if (blanks > 0) {
                    blanks--;
                    useBlank = true;
                } else {
                    continue;
                }
                auto next_node = gaddag_.followArc(node, c);
                if (next_node) {
                    currentWord = c + currentWord; // Prepend vì đi trái
                    used.push_back(Tile(c, useBlank));
                    // Calc score cho new tile
                    int letterScore = Tile::getDefaultScore(c);
                    auto cellType = board.getCell(row, col).type;
                    if (cellType == CellType::DOUBLE_LETTER) letterScore *= 2;
                    else if (cellType == CellType::TRIPLE_LETTER) letterScore *= 3;
                    else if (cellType == CellType::DOUBLE_WORD) wordMult *= 2;
                    else if (cellType == CellType::TRIPLE_WORD) wordMult *= 3;
                    currentScore += letterScore;
                    int next_r = horizontal ? row : row - 1;
                    int next_c = horizontal ? col - 1 : col;
                    gen(next_r, next_c, horizontal, board, rackCount, blanks, next_node, currentWord, used, currentScore, wordMult, plays, limit - 1);
                    auto branch_node = gaddag_.followArc(next_node, '>');
                    if (branch_node) {
                        go(row, col, horizontal, board, rackCount, blanks, branch_node, currentWord, used, currentScore, wordMult, plays, 15);
                    }
                    currentWord.erase(0, 1); // Backtrack
                    used.pop_back();
                    currentScore -= letterScore;
                    if (cellType == CellType::DOUBLE_WORD) wordMult /= 2;
                    else if (cellType == CellType::TRIPLE_WORD) wordMult /= 3;
                }
                if (useBlank) blanks++;
                else rackCount[c]++;
            }
        }
    }
}
// Hàm "Go" - đi sang phải từ điểm neo
void HardStrategy::go(int row, int col, bool horizontal, const Board& board,
                      std::unordered_map<char, int>& rackCount, int& blanks,
                      std::shared_ptr<Utils::Node> node, std::string& currentWord,
                      std::vector<Tile>& used, int& currentScore, int& wordMult,
                      std::vector<Move>& plays, int limit) {
    if (!board.isValidPosition(row, col) || limit <= 0) return;
    if (node->isTerminal && currentWord.length() > 1) {
        // Reverse currentWord vì gen prepend làm reverse left part
        std::string fullWord = currentWord;
        std::reverse(fullWord.begin(), fullWord.end());
        // Tìm vị trí bắt đầu của từ
        int start_r = horizontal ? row : row - (fullWord.length() - 1);
        int start_c = horizontal ? col - (fullWord.length() - 1) : col;
        Move move(fullWord, start_r, start_c, horizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL);
        move.setScore(currentScore * wordMult);
        move.addTileUsed(used);
        plays.push_back(move);
    }
    int next_r = horizontal ? row : row + 1;
    int next_c = horizontal ? col + 1 : col;
    if (!board.isValidPosition(next_r, next_c)) return;
    if (board.hasTile(next_r, next_c)) {
        char letter = board.getTileLetter(next_r, next_c);
        auto next_node = gaddag_.followArc(node, letter);
        if (next_node) {
            currentWord += letter; // Append vì đi phải
            go(next_r, next_c, horizontal, board, rackCount, blanks, next_node, currentWord, used, currentScore, wordMult, plays, limit - 1);
            currentWord.pop_back(); // Backtrack
        }
    } else {
        auto crossSet = computeCrossSet(next_r, next_c, !horizontal, board); // perpVertical
        for (char c = 'A'; c <= 'Z'; ++c) {
            if (crossSet.count(c)) {
                bool useBlank = false;
                if (rackCount[c] > 0) rackCount[c]--;
                else if (blanks > 0) { blanks--; useBlank = true; }
                else continue;
                auto next_node = gaddag_.followArc(node, c);
                if (next_node) {
                    currentWord += c; // Append
                    used.push_back(Tile(c, useBlank));
                    // Calc score cho new tile
                    int letterScore = Tile::getDefaultScore(c);
                    auto cellType = board.getCell(next_r, next_c).type;
                    if (cellType == CellType::DOUBLE_LETTER) letterScore *= 2;
                    else if (cellType == CellType::TRIPLE_LETTER) letterScore *= 3;
                    else if (cellType == CellType::DOUBLE_WORD) wordMult *= 2;
                    else if (cellType == CellType::TRIPLE_WORD) wordMult *= 3;
                    currentScore += letterScore;
                    go(next_r, next_c, horizontal, board, rackCount, blanks, next_node, currentWord, used, currentScore, wordMult, plays, limit - 1);
                    currentWord.pop_back(); // Backtrack
                    used.pop_back();
                    currentScore -= letterScore;
                    if (cellType == CellType::DOUBLE_WORD) wordMult /= 2;
                    else if (cellType == CellType::TRIPLE_WORD) wordMult /= 3;
                }
                if (useBlank) blanks++;
                else rackCount[c]++;
            }
        }
    }
}
// Tính toán tập hợp các chữ cái hợp lệ cho một ô trống
std::unordered_set<char> HardStrategy::computeCrossSet(int row, int col, bool perpVertical, const Board& board) {
    std::unordered_set<char> allowed;
    std::string prefix = "", suffix = "";
    int pr = row, pc = col;
    // Prefix: Lùi trước theo perp dir
    if (perpVertical) { // Scan row up
        while (pr > 0 && board.hasTile(pr - 1, pc)) {
            pr--;
            prefix = board.getTileLetter(pr, pc) + prefix;
        }
    } else { // Scan col left
        while (pc > 0 && board.hasTile(pr, pc - 1)) {
            pc--;
            prefix = board.getTileLetter(pr, pc) + prefix;
        }
    }
    // Suffix: Tiến sau theo perp dir
    pr = row, pc = col;
    if (perpVertical) { // Scan row down
        while (pr < Board::SIZE - 1 && board.hasTile(pr + 1, pc)) {
            pr++;
            suffix += board.getTileLetter(pr, pc);
        }
    } else { // Scan col right
        while (pc < Board::SIZE - 1 && board.hasTile(pr, pc + 1)) {
            pc++;
            suffix += board.getTileLetter(pr, pc);
        }
    }
    if (prefix.empty() && suffix.empty()) {
        for (char c = 'A'; c <= 'Z'; ++c) allowed.insert(c);
        return allowed;
    }
    for (char c = 'A'; c <= 'Z'; ++c) {
        if (dictionary_.contains(prefix + c + suffix)) {
            allowed.insert(c);
        }
    }
    return allowed;
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
} // namespace Strategies
} // namespace AI