#pragma once
#include "Tile.h"
#include "Board.h"
#include "TileBag.h"
#include <vector>
#include <string>

class Player {
public:
    static const int MAX_RACK_SIZE = 7;
    
    // === Constructors ===
    Player(const std::string& name = "Player");
    
    // === Rack Management ===
    const std::vector<Tile>& getRack() const;
    bool addToRack(Tile tile);             // Trả về false nếu rack đầy
    bool removeFromRack(char letter);      // Xóa theo ký tự
    bool hasLetter(char letter) const;     // Kiểm tra có chữ cái
    
    // === Game Actions ===
    struct PlayResult {
        bool success;
        int score;
        std::vector<std::string> wordsFormed;
    };
    PlayResult playWord(Board& board,      // Đặt từ lên bàn
                        const std::string& word,
                        int row, int col,
                        bool horizontal);
    
    bool swapTiles(TileBag& bag,           // Đổi chữ, dùng TileBag thay Board
                   const std::vector<char>& letters);
    void endTurn(Board& board, TileBag& bag);  // Kết thúc lượt, thêm TileBag
    
    // === Scoring ===
    void addScore(int points);
    int getScore() const;
    
    // === State ===
    std::string getName() const;
    bool isRackFull() const;
    int getRackSize() const;

    // === Serialization ===
    void serialize(std::ofstream& file) const;
    void deserialize(std::ifstream& file);

private:
    std::string name_;
    std::vector<Tile> rack_;
    int score_ = 0;
    
    bool canFormWord(const std::string& word) const;
    void refillRack(TileBag& bag);        // Dùng TileBag thay Board
};