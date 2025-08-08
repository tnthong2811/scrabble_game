#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <fstream>
#include "core/Tile.h"
#include "core/Move.h"
#include "core/dictionary/trie_dictionary.hpp"

// Struct này sẽ chứa kết quả chi tiết của việc phân tích một nước đi
struct MoveResult {
    bool isValid = false;
    int score = 0;
    std::vector<std::string> wordsFormed;
    std::string errorMessage;

    // Hàm tiện ích để tạo nhanh một kết quả không hợp lệ
    static MoveResult Invalid(const std::string& reason) {
        MoveResult result;
        result.isValid = false;
        result.errorMessage = reason;
        return result;
    }
};

class Board {
public:
    static const int SIZE = 15;

    enum class CellType {
        NORMAL, DOUBLE_LETTER, TRIPLE_LETTER,
        DOUBLE_WORD, TRIPLE_WORD, CENTER
    };

    struct Cell {
        bool isPremiumUsed = false;
        CellType type = CellType::NORMAL;
        Tile tile;

        bool hasTile() const {
            return tile.getLetter() != ' ';
        }
    };

    // === Constructors & Setup ===
    Board();
    void reset();

    // === HÀM CÔNG KHAI MỚI - LOGIC CHÍNH ===
    MoveResult validateAndScoreMove(const Move& move, const TrieDictionary& dictionary) const;
    void executeMove(const Move& move);

    // === State Queries (Các hàm truy vấn trạng thái vẫn hữu ích) ===
    bool isEmpty() const;
    bool hasTile(int row, int col) const;
    char getTileLetter(int row, int col) const;
    bool isValidPosition(int row, int col) const;
    const Cell& getCell(int row, int col) const;
    // *** THÊM LẠI HÀM NÀY ĐỂ SỬA LỖI BIÊN DỊCH ***
    // Hàm này rất cần thiết cho AI để tìm nước đi hiệu quả.
    bool isAnchor(int row, int col) const;
    bool isAdjacentToTile(int row, int col) const;
    // === Serialization ===
    void serialize(std::ofstream& file) const;
    void deserialize(std::ifstream& file);

private:
    std::vector<std::vector<Cell>> grid_;
    
    // === Private Helpers ===
    int calculateScoreForSingleWord(const std::string& word, int startRow, int startCol, bool isHorizontal) const;
    void initializePremiumSquares();
    std::string getWordAt(int row, int col, bool horizontal) const;
    bool placeTile(int row, int col, Tile tile);
    void markPremiumUsed(int row, int col);
};