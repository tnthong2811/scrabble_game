#include "AI/Utils/GADDAG.h"
#include <algorithm>
#include <string>
#include <stack>  // Để iterative deserialize

namespace AI {
namespace Utils {

GADDAG::GADDAG() : root_(std::make_shared<Node>()) {}

void GADDAG::buildFromDictionary(const TrieDictionary& dictionary) {
    for (const std::string& word : dictionary.getAllWords()) {
        if (word.length() >= 2) {
            addWord(word);
        }
    }
}

void GADDAG::addWord(const std::string& word) {
    // FIX: Sử dụng rev(prefix) > suffix thay vì rev(suffix) > suffix
    for (size_t i = 1; i <= word.length(); ++i) {
        std::string prefix = word.substr(0, i);
        std::string rev_prefix(prefix.rbegin(), prefix.rend());
        std::string suffix = (i < word.length()) ? word.substr(i) : "";  // Suffix empty nếu i == length
        std::string transformed = rev_prefix + ">" + suffix;
        insertWord(transformed);
    }
}

void GADDAG::insertWord(const std::string& word) {
    auto current = root_;
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = std::make_shared<Node>();
        }
        current = current->children[c];
    }
    current->isTerminal = true;
}

std::shared_ptr<Node> GADDAG::followArc(std::shared_ptr<Node> node, char c) const {
    if (!node) {
        return nullptr;
    }
    auto it = node->children.find(c);
    return (it != node->children.end()) ? it->second : nullptr;
}

std::shared_ptr<Node> GADDAG::getRoot() const {
    return root_;
}

bool GADDAG::saveToFile(const std::string& filename) const {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) return false;
    serializeNode(outFile, root_);
    return true;
}

bool GADDAG::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) return false;
    root_ = deserializeNode(inFile);
    return root_ != nullptr;
}

void GADDAG::serializeNode(std::ostream& out, const std::shared_ptr<Node>& node) const {
    out.write(reinterpret_cast<const char*>(&node->isTerminal), sizeof(node->isTerminal));
    size_t numChildren = node->children.size();
    out.write(reinterpret_cast<const char*>(&numChildren), sizeof(numChildren));
    for (const auto& pair : node->children) {
        out.write(&pair.first, sizeof(pair.first));
        serializeNode(out, pair.second);
    }
}

// FIX: Làm iterative để tránh recursion slow/stack overflow
std::shared_ptr<Node> GADDAG::deserializeNode(std::istream& in) {
    struct StackFrame {
        std::shared_ptr<Node> node;
        size_t numChildren;
        size_t childIndex;
        char key;
    };

    std::stack<StackFrame> stack;
    std::shared_ptr<Node> root = std::make_shared<Node>();

    bool isTerminal;
    in.read(reinterpret_cast<char*>(&isTerminal), sizeof(isTerminal));
    if (in.gcount() != sizeof(isTerminal)) return nullptr;
    root->isTerminal = isTerminal;

    size_t numChildren;
    in.read(reinterpret_cast<char*>(&numChildren), sizeof(numChildren));
    if (in.gcount() != sizeof(numChildren)) return nullptr;

    stack.push({root, numChildren, 0, '\0'});

    while (!stack.empty()) {
        StackFrame& frame = stack.top();

        if (frame.childIndex < frame.numChildren) {
            char key;
            in.read(&key, sizeof(key));
            if (in.gcount() != sizeof(key)) return nullptr;
            frame.key = key;

            auto child = std::make_shared<Node>();
            in.read(reinterpret_cast<char*>(&isTerminal), sizeof(isTerminal));
            if (in.gcount() != sizeof(isTerminal)) return nullptr;
            child->isTerminal = isTerminal;

            in.read(reinterpret_cast<char*>(&numChildren), sizeof(numChildren));
            if (in.gcount() != sizeof(numChildren)) return nullptr;

            frame.node->children[frame.key] = child;
            frame.childIndex++;

            stack.push({child, numChildren, 0, '\0'});
        } else {
            stack.pop();
        }
    }

    return root;
}

} // namespace Utils
} // namespace AI