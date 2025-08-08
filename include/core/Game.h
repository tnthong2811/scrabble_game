#pragma once
#include "Board.h"
#include "dictionary/trie_dictionary.hpp"
#include "Player.h"
#include "TileBag.h"
#include "AI/ScrabbleAI.h"
#include <memory>
#include <vector>

class Game {
public:
    enum class State { NOT_STARTED, PLAYING, GAME_OVER };

    Game();
    void startNewGame(int aiCount = 1);
    void endGame();

    // Player actions
    bool playWord(int playerId, const std::string& word, int row, int col, bool horizontal);
    bool swapTiles(int playerId, const std::vector<char>& letters);
    void passTurn(int playerId);

    // Game flow
    void nextTurn();

    // Getters
    State getState() const;
    const Board& getBoard() const;
    Player* getPlayer(int id) const; // Trả về con trỏ để dễ quản lý
    int getCurrentPlayerId() const;
    const TileBag& getTileBag() const;

    // Save/Load
    bool saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);

private:
    Board board_;
    TrieDictionary dictionary_;
    TileBag tileBag_;
    std::vector<std::unique_ptr<Player>> players_;
    // Thêm đối tượng AI làm thành viên của Game để quản lý tập trung
    std::unique_ptr<AI::ScrabbleAI> ai_;
    int currentPlayerId_;
    State state_;
    int consecutivePasses_ = 0; // Thêm biến đếm lượt bỏ qua liên tiếp

    // Helper methods
    void setupPlayers(int aiCount);
    void refillRack(Player& player);
    bool checkGameEnd() const;
    void calculateFinalScores();
    void processAITurn();
};