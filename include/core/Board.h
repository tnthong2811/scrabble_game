#pragma once
#include "Tile.h"
#include "Move.h"
#include <vector>
#include <unordered_set>
#include <string>
#include <fstream>
class Board {
public:
    static const int SIZE = 15;

    enum class CellType {
        NORMAL, DOUBLE_LETTER, TRIPLE_LETTER,
        DOUBLE_WORD, TRIPLE_WORD, CENTER
    };

    struct Cell {
        bool hasTile() const { return letter != ' '; }
        bool isPremiumUsed = false;
        CellType type = CellType::NORMAL;
        char letter = ' ';
        Tile tile; // Lưu Tile tại ô
    };

    // === Constructors ===
    Board();

    // === Core Gameplay ===
    bool placeTile(int row, int col, Tile tile); // Đặt 1 tile
    bool placeWord(const std::string& word, int row, int col, bool horizontal);
    Tile removeTile(int row, int col);           // Gỡ tile (undo)
    void reset();                                // Reset bàn cờ
    void serialize(std::ofstream& file) const;   // Lưu trạng thái
    void deserialize(std::ifstream& file);       // Tải trạng thái

    // === State Queries ===
    bool isEmpty() const;                       // Bàn trống?
    bool hasTile(int row, int col) const;       // Ô có tile?
    char getTileLetter(int row, int col) const; // Lấy ký tự (trả về ' ' nếu trống)
    bool isValidPosition(int row, int col) const; // Ô hợp lệ?
    const Cell& getCell(int row, int col) const; // Thêm phương thức getCell

    // === Word Validation ===
    bool isAnchor(int row, int col) const;      // Ô có phải điểm neo?
    bool isWordConnected(const std::string& word, int row, int col, bool horizontal) const;
    std::vector<std::string> findNewWords(const std::string& mainWord, int row, int col, bool horizontal) const;

    // === Scoring ===
    int calculateWordScore(const std::string& word, int row, int col, bool horizontal) const;

    // === Premium Squares ===
    CellType getCellType(int row, int col) const;
    void markPremiumUsed(int row, int col);     // Đánh dấu ô premium đã dùng

    // === Board Analysis ===
    std::unordered_set<std::string> getAllWords() const;
    bool canPlaceWord(const Move& move) const;
    int calculateScore(const Move& move) const;

    bool isAdjacentToTile(int row, int col) const;
private:
    std::vector<std::vector<Cell>> grid_; // Sử dụng vector Cell thay vì Tile
    void initializePremiumSquares();
};