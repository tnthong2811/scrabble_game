#ifndef TILE_H
#define TILE_H

#include <string>
#include <map>

class Tile {
public:
    // === Constructors ===
    Tile(char letter = ' ', bool isBlank = false);           // Constructor mặc định
    Tile(char letter, int value, bool isBlank = false);      // Constructor với giá trị

    // === Core Getters ===
    char getLetter() const;         // Trả về ký tự hiển thị
    int getValue() const;           // Trả về điểm số
    bool isBlank() const;           // Kiểm tra blank tile
    bool isVowel() const;           // Kiểm tra xem tile có phải nguyên âm không

    // === Setters ===
    void setBlankLetter(char letter); // Đặt ký tự cho blank tile
    void setValue(int value);         // Thay đổi điểm số (cho power-up)

    // === Utility ===
    std::string toString() const;    // Biểu diễn dạng chuỗi "A(1)"
    bool operator==(const Tile& other) const;
    // Static method to access DEFAULT_SCORES
    static int getDefaultScore(char letter);

private:
    char letter_;      // A-Z, ' ' nếu blank
    int value_;        // Điểm số (0-10)
    bool isBlank_;     // Có phải blank tile
    char blankLetter_; // Ký tự đại diện nếu blank

    static const std::map<char, int> DEFAULT_SCORES; // Bảng điểm Scrabble
};

#endif