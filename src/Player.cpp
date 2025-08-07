#include "core/Player.h"
#include <algorithm>
#include <cctype>

Player::Player(const std::string& name) : name_(name) {}

const std::vector<Tile>& Player::getRack() const {
    return rack_;
}

bool Player::addToRack(Tile tile) {
    if (isRackFull()) return false;
    rack_.push_back(tile);
    return true;
}

bool Player::removeFromRack(char letter) {
    auto it = std::find_if(rack_.begin(), rack_.end(),
                           [letter](const Tile& t) { return std::toupper(t.getLetter()) == std::toupper(letter); });
    if (it != rack_.end()) {
        rack_.erase(it);
        return true;
    }
    return false;
}

bool Player::hasLetter(char letter) const {
    return std::find_if(rack_.begin(), rack_.end(),
                        [letter](const Tile& t) { return std::toupper(t.getLetter()) == std::toupper(letter) || t.isBlank(); }) != rack_.end();
}

Player::PlayResult Player::playWord(Board& board, 
                                  const std::string& word,
                                  int row, int col,
                                  bool horizontal) {
    PlayResult result{false, 0, {}};

    if (!canFormWord(word)) return result;

    if (board.placeWord(word, row, col, horizontal)) {
        result.score = board.calculateWordScore(word, row, col, horizontal);
        result.wordsFormed = board.findNewWords(word, row, col, horizontal);
        result.success = true;

        for (char c : word) {
            char upperC = std::toupper(c);
            auto it = std::find_if(rack_.begin(), rack_.end(),
                                   [upperC](const Tile& t) { return std::toupper(t.getLetter()) == upperC || t.isBlank(); });
            if (it != rack_.end()) {
                if (it->isBlank()) {
                    it->setBlankLetter(c); 
                }
                rack_.erase(it);
            }
        }
        addScore(result.score);
    }
    return result;
}

// Đổi chữ
bool Player::swapTiles(TileBag& bag, const std::vector<char>& letters) {
    if (letters.empty() || letters.size() > MAX_RACK_SIZE) return false;

    for (char c : letters) {
        if (!hasLetter(c)) return false;
    }

    for (char c : letters) {
        removeFromRack(c);
        bag.returnTile(Tile(c, Tile::getDefaultScore(c), false)); 
    }
    refillRack(bag);
    return true;
}

void Player::endTurn(Board& board, TileBag& bag) {
    refillRack(bag);
}

bool Player::canFormWord(const std::string& word) const {
    std::vector<Tile> tempRack = rack_;
    int blankCount = std::count_if(tempRack.begin(), tempRack.end(),
                                   [](const Tile& t) { return t.isBlank(); });

    for (char c : word) {
        char upperC = std::toupper(c);
        auto it = std::find_if(tempRack.begin(), tempRack.end(),
                               [upperC](const Tile& t) { return std::toupper(t.getLetter()) == upperC; });
        if (it != tempRack.end()) {
            tempRack.erase(it);
        } else if (blankCount > 0) {
            blankCount--;
        } else {
            return false;
        }
    }
    return true;
}

void Player::refillRack(TileBag& bag) {
    while (!isRackFull() && !bag.isEmpty()) {
        addToRack(bag.drawTile());
    }
}

void Player::addScore(int points) { score_ += points; }
int Player::getScore() const { return score_; }
std::string Player::getName() const { return name_; }
bool Player::isRackFull() const { return rack_.size() >= MAX_RACK_SIZE; }
int Player::getRackSize() const { return rack_.size(); }

void Player::serialize(std::ofstream& file) const {
    file << name_ << "\n";
    file << score_ << "\n";
    for (const auto& tile : rack_) {
        file << tile.getLetter() << " " << tile.getValue() << " " << tile.isBlank() << " ";
    }
    file << "\n";
}

void Player::deserialize(std::ifstream& file) {
    rack_.clear();
    std::string name;
    std::getline(file, name);
    name_ = name;
    file >> score_;
    file.ignore();
    char letter;
    int value;
    bool isBlank;
    while (file >> letter >> value >> isBlank) {
        rack_.emplace_back(letter, value, isBlank);
    }
}