#include "core/TileBag.h"
#include <algorithm>
#include <sstream>
#include <chrono>
#include <stdexcept>

namespace TileDistributions {
    const std::vector<Tile> STANDARD_ENGLISH = {
        Tile(' ', 0, true), Tile(' ', 0, true),
        Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1),
        Tile('B', 3), Tile('B', 3),
        Tile('C', 3), Tile('C', 3),
        Tile('D', 2), Tile('D', 2), Tile('D', 2), Tile('D', 2),
        Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1),
        Tile('F', 4), Tile('F', 4),
        Tile('G', 2), Tile('G', 2), Tile('G', 2),
        Tile('H', 4), Tile('H', 4),
        Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1),
        Tile('J', 8),
        Tile('K', 5),
        Tile('L', 1), Tile('L', 1), Tile('L', 1), Tile('L', 1),
        Tile('M', 3), Tile('M', 3),
        Tile('N', 1), Tile('N', 1), Tile('N', 1), Tile('N', 1), Tile('N', 1), Tile('N', 1),
        Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1),
        Tile('P', 3), Tile('P', 3),
        Tile('Q', 10),
        Tile('R', 1), Tile('R', 1), Tile('R', 1), Tile('R', 1), Tile('R', 1), Tile('R', 1),
        Tile('S', 1), Tile('S', 1), Tile('S', 1), Tile('S', 1),
        Tile('T', 1), Tile('T', 1), Tile('T', 1), Tile('T', 1), Tile('T', 1), Tile('T', 1),
        Tile('U', 1), Tile('U', 1), Tile('U', 1), Tile('U', 1),
        Tile('V', 4), Tile('V', 4),
        Tile('W', 4), Tile('W', 4),
        Tile('X', 8),
        Tile('Y', 4), Tile('Y', 4),
        Tile('Z', 10)
    };
}

TileBag::TileBag() {
    rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());
    initializeStandardSet();
    shuffle();
}

Tile TileBag::drawTile() {
    if (isEmpty()) throw std::runtime_error("TileBag is empty");
    Tile tile = tiles_.back();
    tiles_.pop_back();
    updateLetterCounts(tile, -1);
    return tile;
}

std::vector<Tile> TileBag::drawTiles(int count) {
    if (count < 0) throw std::invalid_argument("Count must be non-negative");
    int numToDraw = std::min(count, static_cast<int>(tiles_.size()));
    std::vector<Tile> drawnTiles;
    drawnTiles.reserve(numToDraw);
    for (int i = 0; i < numToDraw; ++i) {
        drawnTiles.push_back(drawTile());
    }
    return drawnTiles;
}

void TileBag::returnTile(const Tile& tile) {
    tiles_.push_back(tile);
    updateLetterCounts(tile, 1);
}

void TileBag::returnTiles(const std::vector<Tile>& tiles) {
    for (const auto& tile : tiles) {
        returnTile(tile);
    }
}

// === Bag Operations ===
void TileBag::shuffle() {
    std::shuffle(tiles_.begin(), tiles_.end(), rng_);
}

void TileBag::reset() {
    tiles_ = initialSet_;
    letterCounts_.clear();
    for (const auto& tile : initialSet_) {
        updateLetterCounts(tile, 1);
    }
    shuffle();
}

bool TileBag::isEmpty() const {
    return tiles_.empty();
}

int TileBag::remainingTiles() const {
    return static_cast<int>(tiles_.size());
}

int TileBag::initialTileCount() const {
    return static_cast<int>(initialSet_.size());
}

std::map<char, int> TileBag::getLetterDistribution() const {
    return letterCounts_;
}

int TileBag::getRemainingLetterCount(char letter) const {
    auto it = letterCounts_.find(std::toupper(letter));
    return (it != letterCounts_.end()) ? it->second : 0;
}

std::string TileBag::serialize() const {
    std::ostringstream oss;
    for (const auto& tile : tiles_) {
        oss << (tile.isBlank() ? "?" : std::string(1, tile.getLetter())) 
            << tile.getValue() << ' ';
    }
    return oss.str();
}

bool TileBag::deserialize(const std::string& data) {
    std::istringstream iss(data);
    tiles_.clear();
    letterCounts_.clear();

    std::string token;
    while (iss >> token) {
        if (token.empty()) continue;
        try {
            char letter = token[0];
            bool isBlank = (letter == '?');
            if (isBlank) letter = ' ';
            int value = std::stoi(token.substr(1));
            
            Tile tile(letter, value, isBlank);
            tiles_.push_back(tile);
            updateLetterCounts(tile, 1);
        } catch (const std::exception&) {
            return false;
        }
    }
    return true;
}

void TileBag::initializeStandardSet() {
    initialSet_ = TileDistributions::STANDARD_ENGLISH;
    reset(); 
}

void TileBag::initializeFromSet(const std::vector<Tile>& set) {
    initialSet_ = set;
    reset(); 
}

void TileBag::updateLetterCounts(const Tile& tile, int delta) {
    char letter = tile.getLetter();
    if (tile.isBlank()) {
        letter = ' '; 
    }
    letterCounts_[letter] += delta;
    if (letterCounts_[letter] < 0) letterCounts_[letter] = 0;
}
