#pragma once
#include "Tile.h"
#include <vector>
#include <string>
#include <tuple>
#include <functional>

class Move {
public:
    enum class Direction { HORIZONTAL, VERTICAL };

    struct Hash {
        size_t operator()(const Move& m) const {
            size_t h1 = std::hash<std::string>{}(m.getWord());
            size_t h2 = std::hash<int>{}(m.getRow());
            size_t h3 = std::hash<int>{}(m.getCol());
            size_t h4 = std::hash<int>{}(static_cast<int>(m.getDirection()));
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };

    // Constructor
    Move(const std::string& word, int row, int col, Direction dir);
    Move();
    // Getters
    const std::string& getWord() const;
    int getRow() const;
    int getCol() const;
    Direction getDirection() const;
    int getScore() const;
    const std::vector<Tile>& getTilesUsed() const;
    std::vector<std::pair<int, int>> getCoveredPositions() const;
    std::vector<std::pair<int, int>> getAdjacentPositions() const; // Thêm phương thức mới
    
    // Setters
    void setScore(int score);
    void addTileUsed(const Tile& tile);
    void addTileUsed(const std::vector<Tile>& tiles); // Hỗ trợ thêm nhiều Tile
    
    // Validation
    bool isValid() const;
    
    // Helper methods
    bool isAdjacentTo(const Move& other) const;

    bool operator<(const Move& other) const {
        return std::tie(word_, row_, col_, direction_) < 
            std::tie(other.word_, other.row_, other.col_, other.direction_);
    }

    bool operator==(const Move& other) const {
        return row_ == other.row_ && col_ == other.col_ && 
               word_ == other.word_ && direction_ == other.direction_;
    }

private:
    std::string word_;
    int row_;
    int col_;
    Direction direction_;
    int score_;
    std::vector<Tile> tilesUsed_;
};