#ifndef TRIE_DICTIONARY_HPP
#define TRIE_DICTIONARY_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "dictionary_interface.hpp"

struct TrieNode {
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    bool is_end_of_word = false;
};

class TrieDictionary : public DictionaryInterface {
public:
    TrieDictionary();
    ~TrieDictionary() override = default;
    bool load(const std::string& filePath) override;
    void add_word(const std::string& word) override;
    bool contains(const std::string& word) const override;
    std::vector<std::string> find_matches(const std::string& pattern) const override;
    std::vector<std::string> find_possible_words(const std::string& letters) const override;

private:
    std::unique_ptr<TrieNode> root;
    void insert_word(const std::string& word);
    void find_matches_recursive(TrieNode* node, std::string& current_word, const std::string& pattern, size_t pattern_index, std::vector<std::string>& results) const;
    void find_possible_words_recursive(TrieNode* node, std::string& current_word, std::unordered_map<char, int>& available_letters, std::vector<std::string>& results) const;
};

#endif