#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
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
    std::shared_ptr<Node> getRoot() const;  
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

private:
    std::shared_ptr<Node> root_;
    void serializeNode(std::ostream& out, const std::shared_ptr<Node>& node) const;
    std::shared_ptr<Node> deserializeNode(std::istream& in);
};

} // namespace Utils
} // namespace AI