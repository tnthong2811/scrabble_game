#include <iostream>
#include <string>
#include "../include/core/dictionary/trie_dictionary.hpp"
#include "../include/core/dictionary/dictionary_factory.hpp"

int main() {
    // Tạo từ điển
    auto dictionary = DictionaryFactory::create(DictionaryType::TRIE);
    if (!dictionary->load("c:\\scrabble_game\\Scrabble\\assets\\dictionary\\dictionary.txt")) {
        std::cerr << "Failed to load dictionary.txt!" << std::endl;
        return 1;
    }

    std::string word;
    while (true) {
        // Yêu cầu nhập từ
        std::cout << "Enter a word (or 'quit' to exit): ";
        std::getline(std::cin, word);

        // Kiểm tra điều kiện thoát
        if (word == "quit") {
            break;
        }

        // Kiểm tra từ trong từ điển
        if (dictionary->contains(word)) {
            std::cout << "OK - The word '" << word << "' is in the dictionary." << std::endl;
        } else {
            std::cout << "Not Found - The word '" << word << "' is not in the dictionary." << std::endl;
        }
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
}