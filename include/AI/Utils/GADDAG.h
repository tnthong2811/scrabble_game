#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "core/dictionary/trie_dictionary.hpp"

namespace AI {
namespace Utils {

class GADDAG {
public:
    struct Node {
        std::unordered_map<char, std::shared_ptr<Node>> children;
        bool isTerminal = false;
    };

    GADDAG();
    void buildFromDictionary(const TrieDictionary& dictionary);
    std::shared_ptr<Node> followArc(std::shared_ptr<Node> node, char c) const;
    std::shared_ptr<Node> getRoot() const { return root_; }
private:
    std::shared_ptr<Node> root_;
    void addWord(const std::string& word);
    void insertWord(const std::string& word);
};

} // namespace Utils
} // namespace AI