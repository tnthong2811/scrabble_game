#pragma once
#include "Move.h"

class Play {
public:
    Play(const Move& move) : move_(move) {}
    const Move& getMove() const { return move_; }

private:
    Move move_;
};