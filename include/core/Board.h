#pragma once
#include "Tile.h"
#include "Move.h"
#include "Player.h"
#include "dictionary/trie_dictionary.hpp"
#include <vector>
#include <string>
#include <fstream>

// Định nghĩa các loại ô đặc biệt trên bàn cờ
enum class CellType { NORMAL, DOUBLE_LETTER, TRIPLE_LETTER, DOUBLE_WORD, TRIPLE_WORD, CENTER };

// Định nghĩa cấu trúc để trả về kết quả của một nước đi
struct MoveResult {
    bool isValid = false;
    int score = 0;
    std::string errorMessage;
    std::vector<std::string> wordsFormed;
    std::string lettersUsedFromRack;

    static MoveResult Invalid(const std::string& msg) {
        MoveResult res;
        res.isValid = false;
        res.errorMessage = msg;
        return res;
    }
};

class Board {
public:
    static const int SIZE = 15;

    struct Cell {
        Tile tile;
        CellType type = CellType::NORMAL;
        bool isPremiumUsed = false;
        bool hasTile() const { return tile.getLetter() != ' ' && tile.getLetter() != '?'; }
    };
    
    Board();
    void reset();

    MoveResult validateAndScoreMove(const Move& move, const Player& player, const TrieDictionary& dictionary) const;
    void executeMove(const Move& move);
    // Thêm vào phần public
    void placeTileForAI(int row, int col, Tile tile);
    // === Các hàm Getters công khai cho UI và AI ===
    bool hasTile(int row, int col) const;
    const Cell& getCell(int row, int col) const;
    bool isEmpty() const;
    bool isAnchor(int row, int col) const;
    std::string getWordAt(int row, int col, bool horizontal) const;
    bool isValidPosition(int row, int col) const;
    char getTileLetter(int row, int col) const;

    // === Serialization ===
    void serialize(std::ofstream& file) const;
    void deserialize(std::ifstream& file);

private:
    std::vector<std::vector<Cell>> grid_;

    // === Các hàm Private Helpers ===
    void initializePremiumSquares();
    bool isAdjacentToTile(int row, int col) const;
    int calculateScoreForSingleWord(const Move& move, const std::string& word, int startRow, int startCol, bool isHorizontal, const Board& tempBoard) const;
    bool placeTile(int row, int col, Tile tile);
    void markPremiumUsed(int row, int col);
};