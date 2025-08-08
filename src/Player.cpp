#include "core/Player.h"
#include <algorithm>
#include <cctype>
#include <sstream> // Needed for deserialize

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
    std::vector<Tile> newRack;
    std::vector<bool> used(rack_.size(), false);

    // Step 1: Use regular tiles first
    for (size_t i = 0; i < tempWord.length(); ++i) {
        char letter_to_find = std::toupper(tempWord[i]);
        for (size_t j = 0; j < rack_.size(); ++j) {
            if (!used[j] && !rack_[j].isBlank() && rack_[j].getLetter() == letter_to_find) {
                used[j] = true;
                tempWord[i] = '\0'; // Mark letter as used
                break;
            }
        }
    }

    // Step 2: Use blank tiles for any remaining letters
    for (size_t i = 0; i < tempWord.length(); ++i) {
        if (tempWord[i] != '\0') {
             for (size_t j = 0; j < rack_.size(); ++j) {
                if (!used[j] && rack_[j].isBlank()) {
                    used[j] = true;
                    break;
                }
            }
        }
    }

    // Build the new rack from the tiles that were not used
    for(size_t i=0; i < rack_.size(); ++i) {
        if(!used[i]) {
            newRack.push_back(rack_[i]);
        }
    }
    rack_ = newRack;
}

bool Player::hasLetter(char letter) const {
    char upperC = std::toupper(letter);
    return std::any_of(rack_.begin(), rack_.end(), [upperC](const Tile& t) {
        return t.getLetter() == upperC || t.isBlank();
    });
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
        tilesToReturn.emplace_back(c, Tile::getDefaultScore(c));
    }

    // Remove the tiles from this player's rack
    removeTilesFromRack(wordFromLetters);

    // Return the tiles to the bag
    bag.returnTiles(tilesToReturn);

    // The Game class is responsible for refilling the rack after this returns true
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
        file << (tile.isBlank() ? '?' : tile.getLetter()) << " ";
    }
    file << "\n";
}

void Player::deserialize(std::ifstream& file) {
    rack_.clear();
    std::getline(file, name_);
    file >> score_;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Safely ignore rest of line

    std::string rackData;
    std::getline(file, rackData);
    std::stringstream ss(rackData);
    char letter;
    while(ss >> letter) {
        if(letter == '?') {
            addToRack(Tile(' ', 0, true));
        } else {
            addToRack(Tile(letter, Tile::getDefaultScore(letter)));
        }
    }
}