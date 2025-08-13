// include/Core/dictionary/trie_dictionary.hpp
#pragma once
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
    std::vector<std::string> getAllWords() const; 
    std::vector<std::string> find_possible_words_with_blank(const std::string& letters, int blankCount) const;
    void find_possible_with_blank_recursive(TrieNode* node, std::string& current_word, std::unordered_map<char, int>& available_letters, int blanks, std::vector<std::string>& results) const;

private:
    std::unique_ptr<TrieNode> root;
    void insert_word(const std::string& word);
    void find_matches_recursive(TrieNode* node, std::string& current_word, const std::string& pattern, size_t pattern_index, std::vector<std::string>& results) const;
    void find_possible_words_recursive(TrieNode* node, std::string& current_word, std::unordered_map<char, int>& available_letters, std::vector<std::string>& results) const;
    void getAllWordsRecursive(TrieNode* node, std::string& current_word, std::vector<std::string>& results) const; // Hỗ trợ getAllWords
};