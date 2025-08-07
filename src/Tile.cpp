#include "core/Tile.h"
#include <stdexcept>
#include <algorithm>
#include <cctype> 

const std::map<char, int> Tile::DEFAULT_SCORES = {
    {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1},
    {'F', 4}, {'G', 2}, {'H', 4}, {'I', 1}, {'J', 8},
    {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1}, {'O', 1},
    {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1},
    {'U', 1}, {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4},
    {'Z', 10}, {' ', 0} 
};

Tile::Tile(char letter, bool isBlank)
    : letter_(toupper(letter)), isBlank_(isBlank), blankLetter_(' ') {
    if (isBlank) {
        value_ = 0; 
        letter_ = ' '; 
    } else {
        auto it = DEFAULT_SCORES.find(toupper(letter));
        value_ = (it != DEFAULT_SCORES.end()) ? it->second : 0;
    }
}

Tile::Tile(char letter, int value, bool isBlank)
    : letter_(toupper(letter)), value_(value), isBlank_(isBlank), blankLetter_(' ') {
    if (isBlank) {
        this->value_ = 0;
        letter_ = ' '; 
    } else if (value <= 0) {
        auto it = DEFAULT_SCORES.find(toupper(letter));
        value_ = (it != DEFAULT_SCORES.end()) ? it->second : 0;
    }
}

char Tile::getLetter() const {
    return isBlank_ ? blankLetter_ : letter_;
}

int Tile::getValue() const {
    return value_;
}

bool Tile::isBlank() const {
    return isBlank_;
}

bool Tile::isVowel() const {
    char c = std::tolower(isBlank_ ? blankLetter_ : letter_);
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

void Tile::setBlankLetter(char letter) {
    if (!isBlank_) throw std::runtime_error("Cannot set blank letter on non-blank tile");
    if (letter < 'A' || letter > 'Z') throw std::runtime_error("Blank letter must be A-Z");
    blankLetter_ = toupper(letter);
}

void Tile::setValue(int value) {
    value_ = std::max(0, value); 
}

std::string Tile::toString() const {
    if (isBlank_) {
        return "?[" + std::string(1, blankLetter_) + "](" + std::to_string(value_) + ")";
    }
    return std::string(1, letter_) + "(" + std::to_string(value_) + ")";
}

bool Tile::operator==(const Tile& other) const {
    if (isBlank_ != other.isBlank_) return false;
    if (isBlank_) {
        return blankLetter_ == other.blankLetter_ && value_ == other.value_;
    }
    return letter_ == other.letter_ && value_ == other.value_;
}

int Tile::getDefaultScore(char letter) {
    auto it = DEFAULT_SCORES.find(toupper(letter));
    return (it != DEFAULT_SCORES.end()) ? it->second : 0;
}