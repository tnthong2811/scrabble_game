#include "core/Move.h"
#include <algorithm>

Move::Move()
    : word_(""), row_(-1), col_(-1), direction_(Direction::HORIZONTAL), score_(0) {}
Move::Move(const std::string& word, int row, int col, Direction dir)
    : word_(word), row_(row), col_(col), direction_(dir), score_(0) {}

// Getters
const std::string& Move::getWord() const { return word_; }
int Move::getRow() const { return row_; }
int Move::getCol() const { return col_; }
Move::Direction Move::getDirection() const { return direction_; }
int Move::getScore() const { return score_; }
const std::vector<Tile>& Move::getTilesUsed() const { return tilesUsed_; }

// Setters
void Move::setScore(int score) { score_ = score; }
void Move::addTileUsed(const Tile& tile) { tilesUsed_.push_back(tile); }
void Move::addTileUsed(const std::vector<Tile>& tiles) {
    tilesUsed_.insert(tilesUsed_.end(), tiles.begin(), tiles.end()); 
}

bool Move::isValid() const {
    return !word_.empty() && row_ >= 0 && col_ >= 0 && score_ >= 0;
}

bool Move::isAdjacentTo(const Move& other) const {
    auto myPositions = getCoveredPositions();
    auto otherPositions = other.getCoveredPositions();
    
    for (const auto& pos1 : myPositions) {
        for (const auto& pos2 : otherPositions) {
            if (abs(pos1.first - pos2.first) <= 1 && 
                abs(pos1.second - pos2.second) <= 1) {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::pair<int, int>> Move::getCoveredPositions() const {
    std::vector<std::pair<int, int>> positions;
    for (size_t i = 0; i < word_.length(); ++i) {
        if (direction_ == Direction::HORIZONTAL) {
            positions.emplace_back(row_, col_ + i);
        } else {
            positions.emplace_back(row_ + i, col_);
        }
    }
    return positions;
}

std::vector<std::pair<int, int>> Move::getAdjacentPositions() const {
    std::vector<std::pair<int, int>> adjacent;
    auto covered = getCoveredPositions();
    for (const auto& pos : covered) {
        int r = pos.first, c = pos.second;
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue; 
                adjacent.emplace_back(r + dr, c + dc);
            }
        }
    }
    adjacent.erase(
        std::remove_if(adjacent.begin(), adjacent.end(),
            [](const std::pair<int, int>& pos) {
                return pos.first < 0 || pos.first >= 15 || pos.second < 0 || pos.second >= 15;
            }),
        adjacent.end()
    );
    return adjacent;
}