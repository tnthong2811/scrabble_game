#pragma once
#include "Tile.h"
#include <vector>
#include <random>
#include <map>
#include <string>

// SỬA LỖI 1: Chỉ khai báo biến, không định nghĩa.
// 'extern' báo cho trình biên dịch rằng định nghĩa của biến này nằm ở file khác.
namespace TileDistributions {
    extern const std::vector<Tile> STANDARD_ENGLISH;
}

class TileBag {
public:
    // === Constructors ===
    TileBag();

    // === Tile Management ===
    Tile drawTile();
    std::vector<Tile> drawTiles(int count);

    // SỬA LỖI 2: Thống nhất chữ ký hàm để khớp với file .cpp
    void returnTile(const Tile& tile);
    void returnTiles(const std::vector<Tile>& tiles);

    // === Bag Operations ===
    void shuffle();
    void reset();

    // === State Queries ===
    bool isEmpty() const;
    int remainingTiles() const;
    int initialTileCount() const;

    // === Statistical Info ===
    std::map<char, int> getLetterDistribution() const;
    int getRemainingLetterCount(char letter) const;

    // === Serialization ===
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    std::vector<Tile> tiles_;
    std::vector<Tile> initialSet_;
    std::mt19937 rng_;
    std::map<char, int> letterCounts_;

    void initializeStandardSet();
    void initializeFromSet(const std::vector<Tile>& set);
    void updateLetterCounts(const Tile& tile, int delta);
};
