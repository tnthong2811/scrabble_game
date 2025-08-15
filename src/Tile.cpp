#include "core/Tile.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>

// Bảng điểm chuẩn
const std::map<char, int> Tile::DEFAULT_SCORES = {
    {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1}, {'F', 4}, {'G', 2},
    {'H', 4}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1},
    {'O', 1}, {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1},
    {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Z', 10}, {' ', 0}
};

// Constructor mặc định: Tạo một ô "null" hoặc trống trên bàn cờ
Tile::Tile() : letter_(' '), value_(0), isBlank_(false), blankLetter_('\0') {}

// Constructor chính: Tạo một ô chữ (thường hoặc blank)
Tile::Tile(char letter, bool isBlank) : isBlank_(isBlank) {
    if (isBlank_) {
        letter_ = '?'; // Dùng '?' làm ký tự gốc cho tile blank
        value_ = 0;
        blankLetter_ = '\0'; // Ban đầu chưa đại diện cho chữ nào
    } else {
        letter_ = std::toupper(letter);
        value_ = getDefaultScore(letter_);
        blankLetter_ = '\0';
    }
}

// Constructor phụ
Tile::Tile(char letter, int value, bool isBlank) 
    : letter_(letter), value_(value), isBlank_(isBlank), assignedLetter_(' ') {} // Khởi tạo assignedLetter_

char Tile::getLetter() const {
    // Nếu là quân trắng và đã được gán chữ, trả về chữ được gán
    if (isBlank_ && assignedLetter_ != ' ') {
        return assignedLetter_;
    }
    // Ngược lại, trả về chữ gốc
    return letter_;
}

void Tile::assignLetter(char c) {
    if (isBlank_) {
        assignedLetter_ = toupper(c);
    }
}


// Trả về điểm số
int Tile::getValue() const {
    return isBlank_ ? 0 : value_; // Blank tile luôn có giá trị 0
}

// Kiểm tra có phải blank tile không
bool Tile::isBlank() const {
    return isBlank_;
}

// Gán một chữ cái cho blank tile
void Tile::setBlankLetter(char letter) {
    if (isBlank_) {
        blankLetter_ = std::toupper(letter);
    }
}

// Các hàm còn lại
bool Tile::isVowel() const {
    char c = std::tolower(getLetter()); // Dùng getLetter() để xử lý đúng cho cả blank tile
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

void Tile::setValue(int value) {
    value_ = std::max(0, value);
}

std::string Tile::toString() const {
    if (isBlank_) {
        return "?(" + std::string(1, getLetter()) + ")";
    }
    return std::string(1, letter_) + "(" + std::to_string(value_) + ")";
}

bool Tile::operator==(const Tile& other) const {
    if (isBlank_ != other.isBlank_) return false;
    return letter_ == other.letter_ && value_ == other.value_;
}

int Tile::getDefaultScore(char letter) {
    auto it = DEFAULT_SCORES.find(std::toupper(letter));
    return (it != DEFAULT_SCORES.end()) ? it->second : 0;
}