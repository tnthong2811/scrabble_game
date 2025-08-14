#include "core/dictionary/trie_dictionary.hpp"
#include "AI/Utils/GADDAG.h"
#include <iostream>

int main() {
    TrieDictionary dictionary;
    std::cout << "Loading dictionary..." << std::endl;
    if (!dictionary.load("assets/dictionary/dictionary.txt")) {
        std::cerr << "Failed to load dictionary." << std::endl;
        return 1;
    }

    AI::Utils::GADDAG gaddag;
    std::cout << "Building GADDAG... (This may take a few minutes)" << std::endl;
    gaddag.buildFromDictionary(dictionary);

    std::cout << "Saving GADDAG to file..." << std::endl;
    if (gaddag.saveToFile("assets/gaddag.dat")) {
        std::cout << "GADDAG saved successfully to assets/gaddag.dat" << std::endl;
    } else {
        std::cerr << "Failed to save GADDAG." << std::endl;
        return 1;
    }

    return 0;
}