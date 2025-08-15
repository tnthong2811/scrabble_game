#pragma once
#include <string>
#include <map>

class Tile {
public:
    // === Constructors ===
    Tile(); // Constructor mặc định cho một ô trống (ví dụ: trên bàn cờ)
    Tile(char letter, bool isBlank); // Constructor chính để tạo ô chữ
    Tile(char letter, int value, bool isBlank); // Constructor phụ (tùy chọn)

    // === Core Getters ===
    char getLetter() const;
    int getValue() const;
    bool isBlank() const;
    bool isVowel() const;

    // === Setters ===
    void setBlankLetter(char letter);
    void setValue(int value);

    // === Utility ===
    std::string toString() const;
    bool operator==(const Tile& other) const;
    static int getDefaultScore(char letter);
    void assignLetter(char c);

private:
    char letter_;
    int value_;
    bool isBlank_;
    char blankLetter_;
    char assignedLetter_;

    static const std::map<char, int> DEFAULT_SCORES;
};