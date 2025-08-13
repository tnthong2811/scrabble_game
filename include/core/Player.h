#pragma once
#include "Tile.h"
#include "TileBag.h"
#include "dictionary/trie_dictionary.hpp"
#include <vector>
#include <functional>
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
    bool canFormWord(const std::string& word) const;
    std::string findTwoLetterWord(const TrieDictionary& dictionary) const;
    std::string findValidWord(const TrieDictionary& dictionary) const;
    // === Game Actions ===
    bool swapTiles(TileBag& bag, const std::vector<char>& letters);
    std::vector<char> selectSwapTiles() const;

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