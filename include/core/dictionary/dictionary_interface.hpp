#ifndef DICTIONARY_INTERFACE_HPP
#define DICTIONARY_INTERFACE_HPP

#include <string>
#include <vector>

class DictionaryInterface {
public:
    virtual ~DictionaryInterface() = default;
    virtual bool load(const std::string& filePath) = 0;
    virtual void add_word(const std::string& word) = 0;
    virtual bool contains(const std::string& word) const = 0;
    virtual std::vector<std::string> find_matches(const std::string& pattern) const = 0;
    virtual std::vector<std::string> find_possible_words(const std::string& letters) const = 0;
};

#endif