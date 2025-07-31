#pragma once
#include "Tile.h"
#include <vector>
#include <string>

class Move {
public:
    enum class Direction { HORIZONTAL, VERTICAL };

    // Constructor
    Move(const std::string& word, int row, int col, Direction dir);
    
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

private:
    std::string word_;
    int row_;
    int col_;
    Direction direction_;
    int score_;
    std::vector<Tile> tilesUsed_;
};