#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "Core/dictionary/trie_dictionary.hpp"
#include "Core/Board.h"
#include "Core/Tile.h"
#include "Core/Move.h"

namespace AI {
namespace Utils {

class GADDAG {
public:
    struct Node {
        std::unordered_map<char, std::shared_ptr<Node>> children;
        bool isTerminal = false;
    };

    struct BoardPosition {
        int row;
        int col;
        BoardPosition(int r, int c) : row(r), col(c) {}
    };

    GADDAG();
    void buildFromDictionary(const TrieDictionary& dictionary); 
    bool isWordValid(const std::string& word) const;
    std::vector<std::string> findValidWords(const std::vector<char>& letters) const;
    std::vector<std::string> findValidWords(const BoardPosition& pos, bool horizontal, const Board& board,
                                           const std::vector<char>& letters, const std::vector<bool>& isBlank) const;
    std::vector<Move> generatePlaysForPosition(const Board& board, const BoardPosition& position,
                                             bool horizontal, const std::vector<Tile>& rack) const;
    std::vector<std::string> findWordsWithPattern(const std::string& pattern, const std::vector<char>& letters,
                                                 const std::vector<bool>& isBlank) const;

private:
    std::shared_ptr<Node> root_;

    void addWord(const std::string& word);
    void insertWord(const std::string& word);
    bool searchWord(const std::string& word) const;
    void traverse(const std::shared_ptr<Node>& node, std::vector<std::string>& results, std::string currentWord,
                  const std::vector<char>& availableLetters, std::vector<bool>& used, int blankCount) const;
    int calculateScore(const std::string& word, const Board& board, const BoardPosition& pos, bool horizontal) const; // Thêm khai báo
};

} // namespace Utils
} // namespace AI