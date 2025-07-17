#pragma once
#include "dictionary_interface.hpp"
#include <memory>

using DictionaryPtr = std::unique_ptr<DictionaryInterface>;

enum class DictionaryType {
    TRIE,
    // Có thể thêm các loại khác sau này
    // HASH,
    // DAWG
};

class DictionaryFactory {
public:
    static DictionaryPtr create(DictionaryType type);
};

inline DictionaryPtr DictionaryFactory::create(DictionaryType type) {
    switch (type) {
        case DictionaryType::TRIE:
            return std::make_unique<TrieDictionary>();
        default:
            throw std::invalid_argument("Unsupported dictionary type");
    }
}