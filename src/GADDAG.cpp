#include "AI/Utils/GADDAG.h"
#include <algorithm>
#include <string>

namespace AI {
namespace Utils {

// Constructor và các hàm xây dựng cây được giữ nguyên
GADDAG::GADDAG() : root_(std::make_shared<Node>()) {}

void GADDAG::buildFromDictionary(const TrieDictionary& dictionary) {
    for (const std::string& word : dictionary.getAllWords()) {
        if (word.length() >= 2) {
            addWord(word);
        }
    }
}

void GADDAG::addWord(const std::string& word) {
    std::string reversed(word.rbegin(), word.rend());
    for (size_t i = 1; i <= word.length(); ++i) {
        std::string transformed = reversed.substr(0, i) + ">" + word.substr(word.length() - i);
        insertWord(transformed);
    }
}

void GADDAG::insertWord(const std::string& word) {
    auto current = root_;
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = std::make_shared<Node>();
        }
        current = current->children[c];
    }
    current->isTerminal = true;
}


// *** TRIỂN KHAI HÀM HELPER MỚI ***
std::shared_ptr<GADDAG::Node> GADDAG::followArc(std::shared_ptr<Node> node, char c) const {
    if (!node) {
        return nullptr;
    }
    auto it = node->children.find(c);
    return (it != node->children.end()) ? it->second : nullptr;
}

} // namespace Utils
} // namespace AI