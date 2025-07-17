#include "../include/core/dictionary/trie_dictionary.hpp"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <unordered_map>

TrieDictionary::TrieDictionary() : root(std::make_unique<TrieNode>()) {}

bool TrieDictionary::load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string word;
    while (std::getline(file, word)) {
        if (word.empty()) continue;
        word.erase(std::remove_if(word.begin(), word.end(), 
                  [](char c) { return !std::isalpha(c) && c != '\''; }), 
                  word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::toupper);
        
        if (!word.empty()) {
            insert_word(word);
        }
    }
    file.close();
    return true;
}

void TrieDictionary::insert_word(const std::string& word) {
    TrieNode* current = root.get();
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = std::make_unique<TrieNode>();
        }
        current = current->children[c].get();
    }
    current->is_end_of_word = true;
}

void TrieDictionary::add_word(const std::string& word) {
    std::string normalized = word;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::toupper);
    insert_word(normalized);
}

bool TrieDictionary::contains(const std::string& word) const {
    if (word.empty()) return false;
    TrieNode* current = root.get();
    for (char c : word) {
        c = std::toupper(c);
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            return false;
        }
        current = it->second.get();
    }
    return current && current->is_end_of_word;
}

std::vector<std::string> TrieDictionary::find_matches(const std::string& pattern) const {
    std::vector<std::string> results;
    std::string current_word;
    if (!pattern.empty()) {
        find_matches_recursive(root.get(), current_word, pattern, 0, results);
    }
    return results;
}

void TrieDictionary::find_matches_recursive(
    TrieNode* node,
    std::string& current_word,
    const std::string& pattern,
    size_t pattern_index,
    std::vector<std::string>& results
) const {
    if (!node || pattern_index >= pattern.size()) {
        if (node && node->is_end_of_word) {
            results.push_back(current_word);
        }
        return;
    }
    
    char current_char = pattern[pattern_index];
    if (current_char == '?' || current_char == '*') {
        for (const auto& [ch, child] : node->children) {
            current_word.push_back(ch);
            find_matches_recursive(child.get(), current_word, pattern, pattern_index + 1, results);
            current_word.pop_back();
        }
        if (current_char == '*') {
            find_matches_recursive(node, current_word, pattern, pattern_index + 1, results);
        }
    } else {
        auto it = node->children.find(current_char);
        if (it != node->children.end()) {
            current_word.push_back(current_char);
            find_matches_recursive(it->second.get(), current_word, pattern, pattern_index + 1, results);
            current_word.pop_back();
        }
    }
}

std::vector<std::string> TrieDictionary::find_possible_words(const std::string& letters) const {
    std::vector<std::string> results;
    std::unordered_map<char, int> letter_counts;
    std::string current_word;
    
    for (char c : letters) {
        c = std::toupper(c);
        if (std::isalpha(c)) {
            letter_counts[c]++;
        }
    }
    
    find_possible_words_recursive(root.get(), current_word, letter_counts, results);
    return results;
}

void TrieDictionary::find_possible_words_recursive(
    TrieNode* node,
    std::string& current_word,
    std::unordered_map<char, int>& available_letters,
    std::vector<std::string>& results
) const {
    if (!node) return;
    
    if (node->is_end_of_word && !current_word.empty()) {
        results.push_back(current_word);
    }
    
    for (const auto& [ch, child] : node->children) {
        if (available_letters.find(ch) != available_letters.end() && available_letters[ch] > 0) {
            available_letters[ch]--;
            current_word.push_back(ch);
            find_possible_words_recursive(child.get(), current_word, available_letters, results);
            current_word.pop_back();
            available_letters[ch]++;
        }
    }
}