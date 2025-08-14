#include "core/TileBag.h"
#include <algorithm>
#include <sstream>
#include <random>
#include <stdexcept>

namespace TileDistributions {
    const std::vector<Tile> STANDARD_ENGLISH = {
        Tile(' ', 0, true), Tile(' ', 0, true),  // Blank tiles (dùng ' ' để rõ ràng, constructor sẽ override thành '?')

        // A's (9 tiles)
        Tile('A', 1, false), Tile('A', 1, false), Tile('A', 1, false), Tile('A', 1, false), Tile('A', 1, false),
        Tile('A', 1, false), Tile('A', 1, false), Tile('A', 1, false), Tile('A', 1, false),

        // B's (2)
        Tile('B', 3, false), Tile('B', 3, false),

        // C's (2)
        Tile('C', 3, false), Tile('C', 3, false),

        // D's (4)
        Tile('D', 2, false), Tile('D', 2, false), Tile('D', 2, false), Tile('D', 2, false),

        // E's (12)
        Tile('E', 1, false), Tile('E', 1, false), Tile('E', 1, false), Tile('E', 1, false), Tile('E', 1, false),
        Tile('E', 1, false), Tile('E', 1, false), Tile('E', 1, false), Tile('E', 1, false), Tile('E', 1, false),
        Tile('E', 1, false), Tile('E', 1, false),

        // F's (2)
        Tile('F', 4, false), Tile('F', 4, false),

        // G's (3)
        Tile('G', 2, false), Tile('G', 2, false), Tile('G', 2, false),

        // H's (2)
        Tile('H', 4, false), Tile('H', 4, false),

        // I's (9)
        Tile('I', 1, false), Tile('I', 1, false), Tile('I', 1, false), Tile('I', 1, false), Tile('I', 1, false),
        Tile('I', 1, false), Tile('I', 1, false), Tile('I', 1, false), Tile('I', 1, false),

        // J (1)
        Tile('J', 8, false),

        // K (1)
        Tile('K', 5, false),

        // L's (4)
        Tile('L', 1, false), Tile('L', 1, false), Tile('L', 1, false), Tile('L', 1, false),

        // M's (2)
        Tile('M', 3, false), Tile('M', 3, false),

        // N's (6)
        Tile('N', 1, false), Tile('N', 1, false), Tile('N', 1, false), Tile('N', 1, false), Tile('N', 1, false), Tile('N', 1, false),

        // O's (8)
        Tile('O', 1, false), Tile('O', 1, false), Tile('O', 1, false), Tile('O', 1, false), Tile('O', 1, false),
        Tile('O', 1, false), Tile('O', 1, false), Tile('O', 1, false),

        // P's (2)
        Tile('P', 3, false), Tile('P', 3, false),

        // Q (1)
        Tile('Q', 10, false),

        // R's (6)
        Tile('R', 1, false), Tile('R', 1, false), Tile('R', 1, false), Tile('R', 1, false), Tile('R', 1, false), Tile('R', 1, false),

        // S's (4)
        Tile('S', 1, false), Tile('S', 1, false), Tile('S', 1, false), Tile('S', 1, false),

        // T's (6)
        Tile('T', 1, false), Tile('T', 1, false), Tile('T', 1, false), Tile('T', 1, false), Tile('T', 1, false), Tile('T', 1, false),

        // U's (4)
        Tile('U', 1, false), Tile('U', 1, false), Tile('U', 1, false), Tile('U', 1, false),

        // V's (2)
        Tile('V', 4, false), Tile('V', 4, false),

        // W's (2)
        Tile('W', 4, false), Tile('W', 4, false),

        // X (1)
        Tile('X', 8, false),

        // Y's (2)
        Tile('Y', 4, false), Tile('Y', 4, false),

        // Z (1)
        Tile('Z', 10, false)
    };
}

TileBag::TileBag() {
    // 1. Tạo một "master list" các ô chữ
    initialSet_ = TileDistributions::STANDARD_ENGLISH;
    // 2. Reset túi về trạng thái ban đầu
    reset();
}

Tile TileBag::drawTile() {
    if (isEmpty()) {
        throw std::runtime_error("Attempted to draw from an empty tile bag.");
    }

    // Lấy ô chữ cuối cùng
    Tile tileToDraw = tiles_.back();
    // Xóa nó khỏi túi
    tiles_.pop_back();

    // Cập nhật bộ đếm
    updateLetterCounts(tileToDraw, -1);

    // Trả về ô chữ đã rút
    return tileToDraw;
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
    // *** ĐÂY LÀ DÒNG QUAN TRỌNG NHẤT ***
    // Sao chép toàn bộ các ô chữ từ "master list" vào túi chữ đang hoạt động
    tiles_ = initialSet_;

    // Xáo trộn túi chữ
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tiles_.begin(), tiles_.end(), g);

    // Cập nhật lại bộ đếm chữ cái (nếu bạn cần)
    letterCounts_.clear();
    for(const auto& tile : tiles_){
        updateLetterCounts(tile, 1);
    }
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
            int value = (token.length() > 1) ? std::stoi(token.substr(1)) : Tile::getDefaultScore(letter);
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
