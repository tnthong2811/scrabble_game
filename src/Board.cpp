#include "core/Board.h"
#include "core/Player.h"
#include <set> // Dùng std::set để tự động loại bỏ các từ bị trùng lặp
#include <algorithm>
#include <cctype>

// =================================================================================
// === KHỐI LOGIC MỚI - TRÁI TIM CỦA VIỆC XÁC THỰC VÀ TÍNH ĐIỂM ====================
// =================================================================================

/**
 * @brief Phân tích một nước đi: kiểm tra tính hợp lệ, tìm tất cả các từ tạo thành và tính điểm tổng.
 * Hàm này KHÔNG làm thay đổi trạng thái của bàn cờ hiện tại.
 * @param move Nước đi cần kiểm tra.
 * @param dictionary Từ điển để xác thực các từ.
 * @return Một đối tượng MoveResult chứa kết quả phân tích.
 */

int Board::calculateScoreForSingleWord(const Move& move, const std::string& word, int startRow, int startCol, bool isHorizontal) const {
    int currentWordScore = 0;
    int wordMultiplier = 1;

    for (size_t i = 0; i < word.length(); ++i) {
        int r = startRow + (isHorizontal ? 0 : i);
        int c = startCol + (isHorizontal ? i : 0);
        int letterScore = Tile::getDefaultScore(word[i]);
        
        // Chỉ áp dụng bonus nếu ô đó chưa có tile (tức là tile mới được đặt)
        if (!this->hasTile(r, c)) { 
            switch(this->grid_[r][c].type) {
                case CellType::DOUBLE_LETTER: letterScore *= 2; break;
                case CellType::TRIPLE_LETTER: letterScore *= 3; break;
                case CellType::DOUBLE_WORD:
                case CellType::CENTER:
                    wordMultiplier *= 2; break;
                case CellType::TRIPLE_WORD: wordMultiplier *= 3; break;
                default: break;
            }
        }
        currentWordScore += letterScore;
    }
    return currentWordScore * wordMultiplier;
}

MoveResult Board::validateAndScoreMove(const Move& move, const Player& player, const TrieDictionary& dictionary) const {
    Board tempBoard = *this;
    std::string lettersFromRack;
    std::string fullWord = move.getWord();
    bool isHorizontal = move.getDirection() == Move::Direction::HORIZONTAL;

    for (size_t i = 0; i < fullWord.length(); ++i) {
        int r = move.getRow() + (isHorizontal ? 0 : i);
        int c = move.getCol() + (isHorizontal ? i : 0);
        if (!isValidPosition(r, c)) return MoveResult::Invalid("Tu dat ngoai ban co.");
        if (!this->hasTile(r, c)) {
            lettersFromRack += fullWord[i];
            tempBoard.grid_[r][c].tile = Tile(fullWord[i], false);
        } else if (this->getTileLetter(r, c) != fullWord[i]) {
            return MoveResult::Invalid("Dat chong len chu cai khong khop.");
        }
    }
    
    if (lettersFromRack.empty()) return MoveResult::Invalid("Nuoc di khong dat chu cai moi nao.");
    if (!player.canFormWord(lettersFromRack)) {
        return MoveResult::Invalid("Nguoi choi khong co du chu cai.");
    }

    if (this->isEmpty()) {
        if (!tempBoard.hasTile(SIZE / 2, SIZE / 2)) {
            return MoveResult::Invalid("Luot dau tien phai di qua o trung tam.");
        }
    } else {
        bool isConnected = false;
        for (const auto& pos : move.getCoveredPositions()) {
            if (isAdjacentToTile(pos.first, pos.second)) {
                isConnected = true;
                break;
            }
        }
        if (!isConnected) return MoveResult::Invalid("Tu moi phai ket noi voi mot tu da co.");
    }
    
    std::set<std::string> allNewWords;
    int totalScore = 0;
    std::string mainWord = tempBoard.getWordAt(move.getRow(), move.getCol(), isHorizontal);
    if(mainWord.length() > 1) allNewWords.insert(mainWord);
    
    for (size_t i = 0; i < fullWord.length(); ++i) {
        int r = move.getRow() + (isHorizontal ? 0 : i);
        int c = move.getCol() + (isHorizontal ? i : 0);
        if(!this->hasTile(r, c)) {
            std::string sideWord = tempBoard.getWordAt(r, c, !isHorizontal);
            if(sideWord.length() > 1) allNewWords.insert(sideWord);
        }
    }

    if (allNewWords.empty()) return MoveResult::Invalid("Nuoc di khong tao thanh tu nao.");

    for (const auto& word : allNewWords) {
        if (!dictionary.contains(word)) {
            return MoveResult::Invalid("Tu '" + word + "' khong co trong tu dien.");
        }
    }

    for (const auto& word : allNewWords) {
        bool found = false;
        for (int r = 0; r < SIZE && !found; ++r) for (int c = 0; c < SIZE && !found; ++c) {
            // *** SỬA LỖI: Truyền 'move' vào lời gọi hàm ***
            if (tempBoard.getWordAt(r, c, true) == word) {
                totalScore += calculateScoreForSingleWord(move, word, r, c, true); found = true;
            } else if (tempBoard.getWordAt(r, c, false) == word) {
                totalScore += calculateScoreForSingleWord(move, word, r, c, false); found = true;
            }
        }
    }

    if (lettersFromRack.length() == 7) totalScore += 50;

    MoveResult finalResult;
    finalResult.isValid = true;
    finalResult.score = totalScore;
    finalResult.lettersUsedFromRack = lettersFromRack;
    finalResult.wordsFormed.assign(allNewWords.begin(), allNewWords.end());
    return finalResult;
}

void Board::executeMove(const Move& move) {
    bool isHorizontal = move.getDirection() == Move::Direction::HORIZONTAL;
    const std::string& word = move.getWord();

    for (size_t i = 0; i < word.length(); ++i) {
        int r = move.getRow() + (isHorizontal ? 0 : i);
        int c = move.getCol() + (isHorizontal ? i : 0);

        // Chỉ đặt một tile mới nếu ô đó trên bàn cờ thật đang trống.
        // Điều này đảm bảo chúng ta không đặt đè lên các chữ cái đã có
        // mà từ mới đang nối vào.
        if (!this->hasTile(r, c)) {
            Tile newTile(word[i], false); // Tạo tile mới, không phải blank
            this->placeTile(r, c, newTile); // Sử dụng hàm helper để đặt tile và cập nhật isPremiumUsed
        }
    }
}

/**
 * @brief Thực thi một nước đi lên bàn cờ thật. Chỉ gọi hàm này sau khi đã validate thành công.
 */
// =================================================================================
// === CÁC HÀM PHỤ TRỢ =============================================================
// =================================================================================

Board::Board() {
    grid_.resize(SIZE, std::vector<Cell>(SIZE));
    initializePremiumSquares();
}

void Board::reset() {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            grid_[i][j] = Cell(); // Reset ô về trạng thái mặc định
        }
    }
    initializePremiumSquares();
}

/**
 * @brief Lấy nguyên một từ (chuỗi chữ cái liên tục) tại một vị trí cho trước theo một hướng.
 */
std::string Board::getWordAt(int row, int col, bool horizontal) const {
    if (!grid_[row][col].hasTile()) {
        return "";
    }

    int start = horizontal ? col : row;
    // Lùi về đầu từ
    while (start > 0) {
        int r = horizontal ? row : start - 1;
        int c = horizontal ? start - 1 : col;
        if (!grid_[r][c].hasTile()) break;
        start--;
    }

    std::string word = "";
    // Đọc xuôi để lấy từ
    for (int current = start; current < SIZE; ++current) {
        int r = horizontal ? row : current;
        int c = horizontal ? current : col;
        if (!grid_[r][c].hasTile()) break;
        word += grid_[r][c].tile.getLetter();
    }
    return word;
}

// Hàm này giờ là private, được gọi bởi executeMove
bool Board::placeTile(int row, int col, Tile tile) {
    if (!isValidPosition(row, col) || hasTile(row, col)) return false;
    grid_[row][col].tile = tile;
    // Đánh dấu ô thưởng đã dùng ngay khi đặt
    if (grid_[row][col].type != CellType::NORMAL) {
        markPremiumUsed(row, col);
    }
    return true;
}

void Board::markPremiumUsed(int row, int col) {
    if (isValidPosition(row, col)) {
        grid_[row][col].isPremiumUsed = true;
    }
}

// --- CÁC HÀM GETTER VÀ HÀM CŨ VẪN HỮU ÍCH ---
// (Giữ lại các hàm như isEmpty, hasTile, getTileLetter, isValidPosition, ...)
bool Board::isEmpty() const {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (grid_[i][j].hasTile()) return false;
    return true;
}

bool Board::hasTile(int row, int col) const {
    return isValidPosition(row, col) && grid_[row][col].hasTile();
}

char Board::getTileLetter(int row, int col) const {
    return hasTile(row, col) ? grid_[row][col].tile.getLetter() : ' ';
}

const Board::Cell& Board::getCell(int row, int col) const {
    if (isValidPosition(row, col)) return grid_[row][col];
    static Cell defaultCell;
    return defaultCell;
}

bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

void Board::placeTileForAI(int row, int col, Tile tile) {
    if (isValidPosition(row, col) && !hasTile(row, col)) {
        grid_[row][col].tile = tile;
    }
}
/**
 * @brief Kiểm tra xem một ô có phải là "điểm neo" hay không.
 * Điểm neo là một ô trống và nằm ngay cạnh một ô đã có chữ.
 */
bool Board::isAnchor(int row, int col) const {
    // Một ô là điểm neo nếu nó hợp lệ, trống, VÀ nằm cạnh một ô đã có chữ.
    if (!isValidPosition(row, col) || hasTile(row, col)) {
        return false;
    }
    return isAdjacentToTile(row, col);
}

bool Board::isAdjacentToTile(int row, int col) const {
    const int dirs[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    for (const auto& [dr, dc] : dirs) {
        int r = row + dr, c = col + dc;
        if (isValidPosition(r, c) && hasTile(r, c)) return true;
    }
    return false;
}

void Board::initializePremiumSquares() {
    // Tọa độ chuẩn cho các ô thưởng trên bàn cờ 15x15
    const std::pair<int, int> triple_word[] = {
        {0,0}, {0,7}, {0,14}, {7,0}, {7,14}, {14,0}, {14,7}, {14,14}
    };
    const std::pair<int, int> double_word[] = {
        {1,1}, {2,2}, {3,3}, {4,4}, {1,13}, {2,12}, {3,11}, {4,10},
        {13,1}, {12,2}, {11,3}, {10,4}, {13,13}, {12,12}, {11,11}, {10,10}
    };
    const std::pair<int, int> triple_letter[] = {
        {1,5}, {1,9}, {5,1}, {5,5}, {5,9}, {5,13}, {9,1}, {9,5},
        {9,9}, {9,13}, {13,5}, {13,9}
    };
    const std::pair<int, int> double_letter[] = {
        {0,3}, {0,11}, {2,6}, {2,8}, {3,0}, {3,7}, {3,14}, {6,2}, {6,6},
        {6,8}, {6,12}, {7,3}, {7,11}, {8,2}, {8,6}, {8,8}, {8,12}, {11,0},
        {11,7}, {11,14}, {12,6}, {12,8}, {14,3}, {14,11}
    };

    // Gán loại cho từng ô dựa trên tọa độ
    for (const auto& pos : triple_word)   grid_[pos.first][pos.second].type = CellType::TRIPLE_WORD;
    for (const auto& pos : double_word)   grid_[pos.first][pos.second].type = CellType::DOUBLE_WORD;
    for (const auto& pos : triple_letter) grid_[pos.first][pos.second].type = CellType::TRIPLE_LETTER;
    for (const auto& pos : double_letter) grid_[pos.first][pos.second].type = CellType::DOUBLE_LETTER;

    // Gán ô trung tâm cuối cùng để đảm bảo nó là CENTER
    // (Theo luật, ô này cũng có giá trị như DOUBLE_WORD)
    grid_[SIZE / 2][SIZE / 2].type = CellType::CENTER;
}

// --- CÁC HÀM SERIALIZATION ---
// (Giữ nguyên các hàm serialize và deserialize)
void Board::serialize(std::ofstream& file) const {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            file << (hasTile(i, j) ? grid_[i][j].tile.getLetter() : '_') << (j == SIZE - 1 ? "" : " ");
        }
        file << "\n";
    }
}

void Board::deserialize(std::ifstream& file) {
    // BƯỚC 1: Đảm bảo bàn cờ ở trạng thái sạch sẽ trước khi tải.
    // Việc này sẽ reset tất cả các ô, bao gồm cả isPremiumUsed và các Tile cũ.
    this->reset();

    // BƯỚC 2: Đọc từ file và dựng lại trạng thái bàn cờ.
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            char letter_from_file;
            // Đọc từng ký tự (đã được ngăn cách bởi dấu cách trong file)
            if (file >> letter_from_file) {
                // Kiểm tra ký tự đại diện cho ô trống mà hàm serialize đã ghi ra
                if (letter_from_file != '_') {
                    // Nếu không phải ô trống, tạo một Tile mới và đặt vào ô.
                    grid_[i][j].tile = Tile(letter_from_file, Tile::getDefaultScore(letter_from_file));
                    
                    // QUAN TRỌNG: Nếu một ô có chữ khi tải game,
                    // ô thưởng ở đó cũng phải được coi là đã sử dụng.
                    if (grid_[i][j].type != CellType::NORMAL) {
                        grid_[i][j].isPremiumUsed = true;
                    }
                }
                // Nếu letter_from_file == '_', chúng ta không cần làm gì cả,
                // vì hàm reset() đã tạo ra một ô trống hoàn hảo rồi.
            } else {
                // Nếu không thể đọc đủ 225 ký tự, file có thể đã bị hỏng.
                return;
            }
        }
    }
}