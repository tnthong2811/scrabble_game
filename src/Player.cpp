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

    for(char c : letters) {
        bool isBlank = (c == '?');
        tilesToReturn.emplace_back(isBlank ? '?' : c, isBlank);
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

std::string Player::findTwoLetterWord(const TrieDictionary& dictionary) const {
    if (rack_.size() < 2) return "";

    for (size_t i = 0; i < rack_.size(); ++i) {
        if (rack_[i].isBlank()) continue;
        for (size_t j = 0; j < rack_.size(); ++j) {
            if (i == j || rack_[j].isBlank()) continue;
            
            std::string word = "";
            word += rack_[i].getLetter();
            word += rack_[j].getLetter();
            if (dictionary.contains(word)) return word;  // Check dict
        }
    }
    return "";
}

std::string Player::findValidWord(const TrieDictionary& dictionary) const {
    std::vector<char> letters;
    std::vector<bool> isBlank(rack_.size(), false);
    for (size_t i = 0; i < rack_.size(); ++i) {
        const auto& tile = rack_[i];
        if (tile.isBlank()) {
            isBlank[i] = true;
        } else {
            letters.push_back(tile.getLetter());
        }
    }
    int blankCount = std::count(isBlank.begin(), isBlank.end(), true);
    if (letters.size() + blankCount < 2) return "";

    std::sort(letters.begin(), letters.end());

    // Function to generate and check words with blanks
    std::function<std::string(std::vector<char>& curr, int blanksLeft)> generate = [&](std::vector<char>& curr, int blanksLeft) {
        if (curr.size() >= 2) {
            std::string word(curr.begin(), curr.end());
            std::cout << "Trying word: " << word << std::endl;  // Log
            if (dictionary.contains(word)) {
                return word;  // Found, return
            }
        }
        if (blanksLeft > 0) {
            for (char rep = 'A'; rep <= 'Z'; ++rep) {
                curr.push_back(rep);
                auto res = generate(curr, blanksLeft - 1);
                if (!res.empty()) return res;
                curr.pop_back();
            }
        } else if (!letters.empty()) {
            // Permute non-blank
            do {
                std::vector<char> temp = curr;
                temp.insert(temp.end(), letters.begin(), letters.end());
                std::string word(temp.begin(), temp.end());
                std::cout << "Trying word: " << word << std::endl;  // Log
                if (dictionary.contains(word)) {
                    return word;
                }
            } while (std::next_permutation(letters.begin(), letters.end()));
        }
        return std::string("");
    };

    std::vector<char> emptyCurr;
    auto result = generate(emptyCurr, blankCount);
    return result;
}
