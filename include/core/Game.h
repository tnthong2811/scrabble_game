#pragma once
#include "Board.h"
#include "dictionary/trie_dictionary.hpp"
#include "Player.h"
#include "Play.h"
#include "TileBag.h"
#include "AI/ScrabbleAI.h"
#include "SDL.h"
#include <memory>
#include <vector>

struct TurnRecord {
    std::string playerName;
    std::string word;
    int score;
    bool isPass = false;
    bool isSwap = false; 
};

class Game {
public:
    enum class State { NOT_STARTED, PLAYING, GAME_OVER };

    Game();
    void startNewGame(int aiCount = 1, int gameDurationMinutes = 45);
    void endGame();
    const std::vector<Play>& getSuggestions() const;
    const std::vector<TurnRecord>& getTurnHistory() const;
    // *** SỬA LỖI: Cập nhật chữ ký hàm này để khớp với GameUI và Game.cpp ***
    bool playWord(int playerId, const std::string& wordFromRack, const std::string& fullWord, int row, int col, bool horizontal);
    
    bool swapTiles(int playerId, const std::vector<char>& letters);
    void passTurn(int playerId);

    // Game flow
    void nextTurn();
    void update();

    // Getters
    State getState() const;
    const Board& getBoard() const;
    Player* getPlayer(int id) const;
    int getCurrentPlayerId() const;
    const TileBag& getTileBag() const;
    int getWinnerId() const;

    // Save/Load
    bool saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);

    //Time
    void updateTimers(); 
    Uint32 getTotalTimeRemaining() const;
    Uint32 getTurnTimeRemaining() const;
private:
    Board board_;
    TrieDictionary dictionary_;
    TileBag tileBag_;
    std::vector<std::unique_ptr<Player>> players_;
    std::unique_ptr<AI::ScrabbleAI> ai_;
    std::vector<TurnRecord> turnHistory_;
    std::vector<Play> currentSuggestions_; 
    int currentPlayerId_;
    State state_;
    int consecutivePasses_ = 0;
    int consecutiveSwaps_ = 0;
    Uint32 totalGameTimeRemaining_;   
    Uint32 currentTurnTimeRemaining_; 
    Uint32 lastUpdateTime_;

    // Helper methods
    void setupPlayers(int aiCount);
    void refillRack(Player& player);
    bool checkGameEnd() const;
    void calculateFinalScores();
    void processAITurn();
};