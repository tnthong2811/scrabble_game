#pragma once
#include "Move.h"
#include <vector>

enum class PlayType {
    PASS,
    MOVE
};

class Play {
public:
    Play() : move_(Move("", 0, 0, Move::Direction::HORIZONTAL)), type_(PlayType::MOVE) {} // Constructor mặc định
    Play(const Move& move) : move_(move), type_(PlayType::MOVE) {}
    Move getMove() const { return move_; }
    int getScore() const { return move_.getScore(); } // Giả định score từ Move
    const std::vector<Tile>& getTilesUsed() const { return move_.getTilesUsed(); } // Giả định từ Move
    PlayType getType() const { return type_; }
    static Play createPass() {
        Play play;
        play.type_ = PlayType::PASS;
        return play;
    }

private:
    Move move_;
    PlayType type_; // Thêm thành viên type
};