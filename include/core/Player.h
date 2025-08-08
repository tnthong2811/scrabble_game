#pragma once
#include "Tile.h"
#include "TileBag.h"
#include <vector>
#include <string>
#include <fstream>

class Player {
public:
    static const int MAX_RACK_SIZE = 7;

    Player(const std::string& name = "Player");

    // === Rack Management ===
    const std::vector<Tile>& getRack() const;
    bool addToRack(Tile tile);
    void removeTilesFromRack(const std::string& word);
    bool hasLetter(char letter) const;
    bool canFormWord(const std::string& word) const;

    // === Game Actions ===
    bool swapTiles(TileBag& bag, const std::vector<char>& letters);

    // === Scoring & State ===
    void addScore(int points);
    int getScore() const;
    std::string getName() const;
    bool isRackFull() const;

    // === Serialization ===
    void serialize(std::ofstream& file) const;
    void deserialize(std::ifstream& file);

private:
    std::string name_;
    std::vector<Tile> rack_;
    int score_ = 0;
};