#include "core/Board.h"
#include <algorithm>
#include <cctype>
#include <fstream>

// Khởi tạo bàn cờ
Board::Board() {
    grid_.resize(SIZE, std::vector<Cell>(SIZE));
    initializePremiumSquares();
}

// Đặt tile
bool Board::placeTile(int row, int col, Tile tile) {
    if (!isValidPosition(row, col)) return false;
    if (hasTile(row, col)) return false;

    grid_[row][col].tile = tile;
    grid_[row][col].letter = tile.getLetter();
    if (grid_[row][col].type != CellType::NORMAL) {
        markPremiumUsed(row, col); // Đánh dấu premium đã dùng khi đặt tile
    }
    return true;
}

// Đặt cả từ
bool Board::placeWord(const std::string& word, int row, int col, bool horizontal) {
    if (!isValidPosition(row, col)) return false;

    for (size_t i = 0; i < word.size(); i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);

        if (!isValidPosition(r, c)) return false;
        if (hasTile(r, c) && getTileLetter(r, c) != word[i]) return false;

        if (!hasTile(r, c)) {
            if (word[i] == ' ') {
                // Blank tile
                Tile blankTile(' ', 0, true); // Sử dụng constructor 3 tham số
                blankTile.setBlankLetter('A'); // Cần thêm vào Tile.h
                grid_[r][c].tile = blankTile;
                grid_[r][c].letter = 'A'; // Gán ký tự tạm thời
            } else {
                int value = Tile::getDefaultScore(word[i]); // Sử dụng phương thức tĩnh
                grid_[r][c].tile = Tile(word[i], value);
                grid_[r][c].letter = word[i];
            }
            if (grid_[r][c].type != CellType::NORMAL) {
                markPremiumUsed(r, c);
            }
        }
    }
    return true;
}

// Tính điểm
int Board::calculateWordScore(const std::string& word, int row, int col, bool horizontal) const {
    int score = 0;
    int wordMultiplier = 1;

    for (size_t i = 0; i < word.size(); i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);

        int letterScore = hasTile(r, c) ? grid_[r][c].tile.getValue() : Tile::getDefaultScore(word[i]);
        CellType type = getCellType(r, c);

        if (!grid_[r][c].isPremiumUsed) {
            switch (type) {
                case CellType::DOUBLE_LETTER: letterScore *= 2; break;
                case CellType::TRIPLE_LETTER: letterScore *= 3; break;
                case CellType::DOUBLE_WORD: wordMultiplier *= 2; break;
                case CellType::TRIPLE_WORD: wordMultiplier *= 3; break;
                case CellType::CENTER: wordMultiplier *= 2; break; // Ví dụ bonus cho ô trung tâm
                default: break;
            }
        }
        score += letterScore;
    }

    return score * wordMultiplier;
}

// Kiểm tra điểm neo
bool Board::isAnchor(int row, int col) const {
    if (!isValidPosition(row, col)) return false;
    if (hasTile(row, col)) return false;
    return isAdjacentToTile(row, col);
}

// Đánh dấu ô premium đã dùng
void Board::markPremiumUsed(int row, int col) {
    if (isValidPosition(row, col)) {
        grid_[row][col].isPremiumUsed = true;
    }
}

// Lấy loại ô
Board::CellType Board::getCellType(int row, int col) const {
    if (!isValidPosition(row, col)) return CellType::NORMAL;
    return grid_[row][col].type;
}

// Lấy ô cụ thể
const Board::Cell& Board::getCell(int row, int col) const {
    if (isValidPosition(row, col)) return grid_[row][col];
    static Cell defaultCell;
    return defaultCell;
}

// Reset bàn cờ
void Board::reset() {
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j) {
            grid_[i][j].tile = Tile();
            grid_[i][j].letter = ' ';
            grid_[i][j].isPremiumUsed = false;
        }
    initializePremiumSquares();
}

// Serialize bàn cờ
void Board::serialize(std::ofstream& file) const {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            file << grid_[i][j].letter << (j == SIZE - 1 ? '\n' : ' ');
        }
    }
}

// Deserialize bàn cờ
void Board::deserialize(std::ifstream& file) {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            char letter;
            if (file >> letter) {
                grid_[i][j].letter = letter;
                if (letter != ' ') {
                    grid_[i][j].tile = Tile(letter, Tile::getDefaultScore(letter));
                } else {
                    grid_[i][j].tile = Tile();
                }
            }
        }
    }
}

// Private helpers
void Board::initializePremiumSquares() {
    grid_[SIZE / 2][SIZE / 2].type = CellType::CENTER;

    const std::pair<int, int> tripleWord[] = {{0, 0}, {0, 7}, {7, 0}, {7, 14}, {14, 0}, {14, 7}, {14, 14}, {7, 7}};
    const std::pair<int, int> doubleWord[] = {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {1, 13}, {2, 12}, {3, 11}, {4, 10},
                                             {13, 1}, {12, 2}, {11, 3}, {10, 4}, {13, 13}, {12, 12}, {11, 11}, {10, 10}};
    const std::pair<int, int> tripleLetter[] = {{1, 5}, {1, 9}, {5, 1}, {5, 5}, {5, 9}, {5, 13}, {9, 1}, {9, 5}, {9, 9}, {9, 13}, {13, 5}, {13, 9}};
    const std::pair<int, int> doubleLetter[] = {{0, 3}, {0, 11}, {2, 6}, {2, 8}, {3, 0}, {3, 7}, {3, 14}, {6, 2}, {6, 6}, {6, 8}, {6, 12},
                                               {7, 3}, {7, 11}, {8, 2}, {8, 6}, {8, 8}, {8, 12}, {11, 0}, {11, 7}, {11, 14}, {12, 6}, {12, 8}, {14, 3}, {14, 11}};

    for (const auto& pos : tripleWord) grid_[pos.first][pos.second].type = CellType::TRIPLE_WORD;
    for (const auto& pos : doubleWord) grid_[pos.first][pos.second].type = CellType::DOUBLE_WORD;
    for (const auto& pos : tripleLetter) grid_[pos.first][pos.second].type = CellType::TRIPLE_LETTER;
    for (const auto& pos : doubleLetter) grid_[pos.first][pos.second].type = CellType::DOUBLE_LETTER;
}

bool Board::isAdjacentToTile(int row, int col) const {
    const int dirs[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    for (const auto& [dr, dc] : dirs) {
        int r = row + dr, c = col + dc;
        if (isValidPosition(r, c) && hasTile(r, c)) return true;
    }
    return false;
}

// Các phương thức khác
bool Board::isEmpty() const {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (hasTile(i, j)) return false;
    return true;
}

bool Board::hasTile(int row, int col) const {
    return isValidPosition(row, col) && grid_[row][col].tile.getLetter() != ' ';
}

char Board::getTileLetter(int row, int col) const {
    return isValidPosition(row, col) ? grid_[row][col].tile.getLetter() : ' ';
}

bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

Tile Board::removeTile(int row, int col) {
    if (!isValidPosition(row, col) || !hasTile(row, col)) return Tile();
    Tile tile = grid_[row][col].tile;
    grid_[row][col].tile = Tile();
    grid_[row][col].letter = ' ';
    return tile;
}

// Kiểm tra kết nối từ
bool Board::isWordConnected(const std::string& word, int row, int col, bool horizontal) const {
    for (size_t i = 0; i < word.size(); i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);
        if (!isValidPosition(r, c)) return false;
        if (hasTile(r, c)) continue; // Ô đã có tile, bỏ qua
        if (isAdjacentToTile(r, c)) return true; // Kết nối với tile lân cận
    }
    return false; // Không có kết nối
}

std::vector<std::string> Board::findNewWords(const std::string& mainWord, int row, int col, bool horizontal) const {
    std::vector<std::string> newWords;

    // Tạo bản sao của Board để thử nghiệm mà không thay đổi trạng thái gốc
    Board tempBoard = *this;

    // Thử đặt từ trên bản sao
    if (tempBoard.placeWord(mainWord, row, col, horizontal)) {
        // Thêm từ chính nếu hợp lệ
        newWords.push_back(mainWord);

        // Duyệt qua từng ký tự trong mainWord
        for (size_t i = 0; i < mainWord.size(); ++i) {
            int r = row + (horizontal ? 0 : i);
            int c = col + (horizontal ? i : 0);

            // Kiểm tra từ vuông góc
            bool isVertical = !horizontal;
            int start = (isVertical ? r : c);
            int end = start + (isVertical ? static_cast<int>(mainWord.size()) : 1);
            int fixedCoord = (isVertical ? c : r);

            std::string perpendicularWord;
            for (int j = start; j < end; ++j) {
                int checkR = isVertical ? j : fixedCoord;
                int checkC = isVertical ? fixedCoord : j;
                if (tempBoard.isValidPosition(checkR, checkC) && tempBoard.hasTile(checkR, checkC)) {
                    perpendicularWord += tempBoard.getTileLetter(checkR, checkC);
                } else if (static_cast<size_t>(j - start) == i) { // So sánh với i (size_t)
                    perpendicularWord += mainWord[i];
                } else {
                    break; // Dừng nếu gặp ô trống
                }
            }
            if (!perpendicularWord.empty() && perpendicularWord.length() >= 2) {
                newWords.push_back(perpendicularWord);
            }

            // Kiểm tra các từ lân cận ở hướng vuông góc
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if ((dr == 0 && dc == 0) || (horizontal && dr != 0) || (!horizontal && dc != 0)) continue;
                    int newR = r + dr;
                    int newC = c + dc;
                    if (!tempBoard.isValidPosition(newR, newC) || !tempBoard.hasTile(newR, newC)) continue;

                    // Xây dựng từ từ ô lân cận
                    std::string adjacentWord;
                    int currR = newR, currC = newC;
                    while (tempBoard.isValidPosition(currR, currC) && tempBoard.hasTile(currR, currC)) {
                        adjacentWord += tempBoard.getTileLetter(currR, currC);
                        if (isVertical) currR++; else currC++;
                    }
                    currR = newR - 1; currC = newC - 1;
                    while (tempBoard.isValidPosition(currR, currC) && tempBoard.hasTile(currR, currC)) {
                        adjacentWord = tempBoard.getTileLetter(currR, currC) + adjacentWord;
                        if (isVertical) currR--; else currC--;
                    }
                    if (!adjacentWord.empty() && adjacentWord.length() >= 2) {
                        newWords.push_back(adjacentWord);
                    }
                }
            }
        }
    }

    // Loại bỏ trùng lặp và sắp xếp
    std::sort(newWords.begin(), newWords.end());
    newWords.erase(std::unique(newWords.begin(), newWords.end()), newWords.end());
    return newWords;
}

// Lấy tất cả các từ hiện có trên bàn cờ
std::unordered_set<std::string> Board::getAllWords() const {
    std::unordered_set<std::string> words;

    // Duyệt từng ô trên bàn cờ
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (hasTile(r, c)) {
                // Kiểm tra từ ngang
                std::string horizontalWord;
                int left = c;
                while (left >= 0 && hasTile(r, left)) left--;
                left++;
                int right = c;
                while (right < SIZE && hasTile(r, right)) right++;
                right--;
                for (int j = left; j <= right; ++j) {
                    if (isValidPosition(r, j)) {
                        horizontalWord += getTileLetter(r, j);
                    }
                }
                if (!horizontalWord.empty() && horizontalWord.length() >= 2) {
                    words.insert(horizontalWord);
                }

                // Kiểm tra từ dọc
                std::string verticalWord;
                int up = r;
                while (up >= 0 && hasTile(up, c)) up--;
                up++;
                int down = r;
                while (down < SIZE && hasTile(down, c)) down++;
                down--;
                for (int i = up; i <= down; ++i) {
                    if (isValidPosition(i, c)) {
                        verticalWord += getTileLetter(i, c);
                    }
                }
                if (!verticalWord.empty() && verticalWord.length() >= 2) {
                    words.insert(verticalWord);
                }
            }
        }
    }

    return words;
}

bool Board::canPlaceWord(const Move& move) const {
    std::string word = move.getWord();
    int row = move.getRow();
    int col = move.getCol();
    bool horizontal = (move.getDirection() == Move::Direction::HORIZONTAL);

    if (!isValidPosition(row, col)) return false;

    for (size_t i = 0; i < word.size(); i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);
        if (!isValidPosition(r, c)) return false;
        if (hasTile(r, c) && getTileLetter(r, c) != word[i]) return false;
    }
    return (isEmpty() && move.getCoveredPositions()[0] == std::make_pair(SIZE / 2, SIZE / 2)) 
           || isWordConnected(word, row, col, horizontal);
}

int Board::calculateScore(const Move& move) const {
    return calculateWordScore(move.getWord(), move.getRow(), move.getCol(), 
                             move.getDirection() == Move::Direction::HORIZONTAL);
}