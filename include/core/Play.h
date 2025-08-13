#pragma once
#include "Move.h"

class Play {
public:
    float score = 0.0f; // Điểm heuristic của nước đi, dùng để AI xếp hạng

    // === Constructors & Creators ===
    Play(const Move& move) : move_(move), isPass_(false) {}
    Play() : move_(Move()), isPass_(true) {} // Constructor mặc định tạo ra một lượt Pass

    static Play createPass() {
        return Play(); // Trả về một đối tượng Play mặc định (là Pass)
    }

    // === Public Methods ===
    const Move& getMove() const { return move_; }
    bool isPass() const { return isPass_; }

private:
    Move move_;
    bool isPass_;
};