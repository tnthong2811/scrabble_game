#include "core/Player.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>
#include <limits> // Cần cho deserialize

Player::Player(const std::string& name) : name_(name), score_(0) {}

const std::vector<Tile>& Player::getRack() const {
    return rack_;
}

bool Player::addToRack(Tile tile) {
    if (isRackFull()) return false;
    rack_.push_back(tile);
    return true;
}

void Player::removeTilesFromRack(const std::string& word) {
    std::string tempWord = word;
    for (char c_word : tempWord) {
        char upperC = std::toupper(c_word);
        auto it = std::find_if(rack_.begin(), rack_.end(), [upperC](const Tile& t) {
            return !t.isBlank() && t.getLetter() == upperC;
        });

        if (it != rack_.end()) {
            rack_.erase(it);
        } else {
            auto blank_it = std::find_if(rack_.begin(), rack_.end(), [](const Tile& t) {
                return t.isBlank();
            });
            if (blank_it != rack_.end()) {
                rack_.erase(blank_it);
            }
        }
    }
}

bool Player::canFormWord(const std::string& word) const {
    auto tempRack = this->rack_;
    for (char c : word) {
        char upperC = std::toupper(c);
        auto it = std::find_if(tempRack.begin(), tempRack.end(), [upperC](const Tile& t){
            return !t.isBlank() && t.getLetter() == upperC;
        });

        if (it != tempRack.end()) {
            tempRack.erase(it);
        } else {
            auto blank_it = std::find_if(tempRack.begin(), tempRack.end(), [](const Tile& t){ return t.isBlank(); });
            if (blank_it != tempRack.end()) {
                tempRack.erase(blank_it);
            } else {
                return false;
            }
        }
    }
    return true;
}

bool Player::swapTiles(TileBag& bag, const std::vector<char>& letters) {
    std::string wordFromLetters(letters.begin(), letters.end());
    if (letters.empty() || !canFormWord(wordFromLetters) || letters.size() > static_cast<size_t>(bag.remainingTiles())) {
        return false;
    }

    std::vector<Tile> tilesToReturn;
    for(char c : letters) {
        // Tạo lại tile với isBlank=false vì chúng là các chữ cái cụ thể
        tilesToReturn.emplace_back(c, false);
    }

    removeTilesFromRack(wordFromLetters);
    bag.returnTiles(tilesToReturn);

    return true;
}

void Player::addScore(int points) { score_ += points; }
int Player::getScore() const { return score_; }
std::string Player::getName() const { return name_; }
bool Player::isRackFull() const { return rack_.size() >= MAX_RACK_SIZE; }

void Player::serialize(std::ofstream& file) const {
    file << name_ << "\n";
    file << score_ << "\n";
    for (const auto& tile : rack_) {
        if (tile.isBlank() && tile.getLetter() != '?') {  // Nếu set
            file << "?" << tile.getLetter() << " ";  // e.g., "?A "
        } else {
            file << tile.getLetter() << " ";
        }
    }
}

void Player::deserialize(std::ifstream& file) {
    rack_.clear();
    std::getline(file, name_);
    file >> score_;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string rackData;
    std::getline(file, rackData);
    std::stringstream ss(rackData);
    std::string tileStr;
    while(ss >> tileStr) {
        if (tileStr.empty()) continue;
        if (tileStr[0] == '?') {
            Tile blankTile('?', true);
            if (tileStr.length() > 1) {
                blankTile.setBlankLetter(tileStr[1]);
            }
            addToRack(blankTile);
        } else {
            addToRack(Tile(tileStr[0], false));
        }
    }
}