#include "TileBag.h"
#include <algorithm>
#include <sstream>
#include <chrono>
#include <stdexcept>

// Định nghĩa namespace TileDistributions
namespace TileDistributions {
    const std::vector<Tile> STANDARD_ENGLISH = {
        // Blank tiles (2)
        Tile(' ', 0, true), Tile(' ', 0, true),
        // A-9, B-2, C-2, D-4, E-12, F-2, G-3, H-2, I-9, J-1, K-1, L-4, M-2,
        Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1),
        Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1),
        Tile('B', 3), Tile('B', 3),
        Tile('C', 3), Tile('C', 3),
        Tile('D', 2), Tile('D', 2), Tile('D', 2), Tile('D', 2),
        Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1),
        Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1),
        Tile('E', 1), Tile('E', 1), Tile('E', 1),
        Tile('F', 4), Tile('F', 4),
        Tile('G', 2), Tile('G', 2), Tile('G', 2),
        Tile('H', 4), Tile('H', 4),
        Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1),
        Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1),
        Tile('J', 8),
        Tile('K', 5),
        Tile('L', 1), Tile('L', 1), Tile('L', 1), Tile('L', 1),
        Tile('M', 3), Tile('M', 3),
        // N-6, O-8, P-2, Q-1, R-6, S-4, T-6, U-4, V-2, W-2, X-1, Y-2, Z-1
        Tile('N', 1), Tile('N', 1), Tile('N', 1), Tile('N', 1), Tile('N', 1), Tile('N', 1),
        Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1),
        Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1),
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

    const std::vector<Tile> VIETNAMESE = {
        // Thêm phân phối cho tiếng Việt tại đây nếu cần...
    };
}

// Constructor
TileBag::TileBag() {
    // Seed random number generator
    rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());
    initializeStandardSet();
    shuffle();
}

// === Tile Management ===
Tile TileBag::drawTile() {
    if (isEmpty()) throw std::runtime_error("TileBag is empty");
    Tile tile = tiles_.back();
    tiles_.pop_back();
    updateLetterCounts(tile, -1);
    return tile;
}

std::vector<Tile> TileBag::drawTiles(int count) {
    if (count < 0) throw std::invalid_argument("Count must be non-negative");
    count = std::min(count, remainingTiles());
    std::vector<Tile> drawnTiles;
    for (int i = 0; i < count; ++i) {
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

// === State Queries ===
bool TileBag::isEmpty() const {
    return tiles_.empty();
}

int TileBag::remainingTiles() const {
    return static_cast<int>(tiles_.size());
}

int TileBag::initialTileCount() const {
    return static_cast<int>(initialSet_.size());
}

// === Statistical Info ===
std::map<char, int> TileBag::getLetterDistribution() const {
    return letterCounts_;
}

int TileBag::getRemainingLetterCount(char letter) const {
    auto it = letterCounts_.find(letter);
    return (it != letterCounts_.end()) ? it->second : 0;
}

// === Serialization ===
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
            int value;
            size_t pos;
            value = std::stoi(token.substr(1), &pos); // Kiểm tra lỗi phân tích
            if (pos != token.substr(1).length()) return false; // Đảm bảo không còn ký tự thừa
            Tile tile(letter, value, isBlank);
            tiles_.push_back(tile);
            updateLetterCounts(tile, 1);
        } catch (const std::exception&) {
            return false;
        }
    }
    return true;
}

// === Private Helpers ===
void TileBag::initializeStandardSet() {
    initialSet_ = TileDistributions::STANDARD_ENGLISH;
    tiles_ = initialSet_;
    letterCounts_.clear();
    for (const auto& tile : initialSet_) {
        updateLetterCounts(tile, 1);
    }
}

void TileBag::initializeFromSet(const std::vector<Tile>& set) {
    initialSet_ = set;
    tiles_ = initialSet_;
    letterCounts_.clear();
    for (const auto& tile : initialSet_) {
        updateLetterCounts(tile, 1);
    }
}

void TileBag::updateLetterCounts(const Tile& tile, int delta) {
    char letter = tile.getLetter(); // Sử dụng letter gốc
    letterCounts_[letter] += delta;
    if (letterCounts_[letter] <= 0) letterCounts_[letter] = 0; // Giữ count không âm
}