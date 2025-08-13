#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "core/dictionary/trie_dictionary.hpp"

namespace AI {
namespace Utils {

struct Node {
    std::unordered_map<char, std::shared_ptr<Node>> children;
    bool isTerminal = false;
};

class GADDAG {
public:
    GADDAG();
    void buildFromDictionary(const TrieDictionary& dictionary);
    void addWord(const std::string& word);
    void insertWord(const std::string& word);
    std::shared_ptr<Node> followArc(std::shared_ptr<Node> node, char c) const;
    std::shared_ptr<Node> getRoot() const;  // Getter cho root_

private:
    std::shared_ptr<Node> root_;
};

} // namespace Utils
} // namespace AI