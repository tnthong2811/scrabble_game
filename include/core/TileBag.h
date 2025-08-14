#pragma once
#include "Tile.h"
#include <vector>
#include <random>
#include <map>
#include <string>

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
    std::mt19937 rng_{std::random_device{}()}; 
    std::map<char, int> letterCounts_;

    void initializeStandardSet();
    void initializeFromSet(const std::vector<Tile>& set);
    void updateLetterCounts(const Tile& tile, int delta);
};
