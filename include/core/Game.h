#pragma once
#include "Board.h"
#include "dictionary/trie_dictionary.hpp"
#include "Player.h"
#include "TileBag.h"
#include "Move.h"
#include "AI/ScrabbleAI.h" 
#include <memory>
#include <vector>

class Game {
public:
    enum class State { NOT_STARTED, PLAYING, GAME_OVER };

    // Constructor
    Game();

    // Game setup
    void startNewGame(int aiCount = 1); // Chỉ cần số AI, người chơi cố định là 1
    void endGame();

    // Player actions
    bool playWord(int playerId, const std::string& word, int row, int col, bool horizontal);
    bool swapTiles(int playerId, const std::vector<char>& letters);
    void passTurn(int playerId);

    // Game flow
    void nextTurn();
    void update();

    // Getters
    State getState() const;
    const Board& getBoard() const;
    const Player& getPlayer(int id) const;
    int getCurrentPlayerId() const;
    const TileBag& getTileBag() const;

    // Save/Load
    bool saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);

private:
    Board board_;
    TrieDictionary dictionary_;
    TileBag tileBag_;
    std::vector<std::unique_ptr<Player>> players_; // 1 human + aiCount AI
    int currentPlayerId_;
    State state_;

    // Helper methods
    void setupPlayers(int aiCount);
    void refillRack(Player& player);
    bool checkGameEnd() const;
    void calculateFinalScores();
    void processAITurn();
};