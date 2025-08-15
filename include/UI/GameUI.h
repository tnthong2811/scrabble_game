#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "core/Game.h"
#include "AI/ScrabbleAI.h"
#include "core/Board.h"
#include <vector>
#include <string>
#include <unordered_map> 

struct TempPlacedTile {
    Tile tile;
    int boardRow;
    int boardCol;
    int originalRackIndex;
};

class GameUI {
public:
    GameUI(Game& gameLogic);
    ~GameUI();
    void run();

private:
    enum class UIState {
        MAIN_MENU,
        LOADING,
        PLAYING,
        SELECTING_SWAP,
        GAME_OVER
    };

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* createTileTexture(const Tile& tile);
    SDL_Texture* createLabelTexture(const std::string& label);
    SDL_Texture* createDraggedTileTexture(const Tile& tile);
    TTF_Font* fontNormal_ = nullptr;
    TTF_Font* fontTile_ = nullptr;
    TTF_Font* fontSmall_ = nullptr;
    TTF_Font* fontHistory_ = nullptr;
    TTF_Font* fontIVL_ = nullptr;
    TTF_Font* fontBig_ = nullptr;
    TTF_Font* fontTitle_ = nullptr;
    TTF_Font* fontCoords_ = nullptr;
    Game& game_;
    bool running_ = false;
    UIState currentState_ = UIState::MAIN_MENU;
    SDL_Rect playButtonRect_;
    SDL_Rect skipButtonRect_;
    SDL_Rect boardRect_;
    SDL_Rect rackRect_;
    SDL_Rect buttonsRect_;
    SDL_Rect sidebarRect_;
    SDL_Rect resetButtonRect_;
    SDL_Rect dynamicSubmitButtonRect_; 
    SDL_Rect dynamicPlayButtonRect_;
    SDL_Rect dynamicSkipButtonRect_;  
    SDL_Rect dynamicResetButtonRect_;
    SDL_Rect playerInfoRect_;
    SDL_Rect opponentInfoRect_;
    SDL_Rect tileBagRect_;
    SDL_Rect turnHistoryRect_;
    SDL_Rect suggestionRect_;
    SDL_Rect swapButtonRect_;
    SDL_Rect dynamicSwapButtonRect_;
    SDL_Rect confirmSwapButtonRect_;
    SDL_Rect dynamicConfirmSwapButtonRect_;
    SDL_Rect replayButtonRect_;
    SDL_Rect dynamicReplayButtonRect_;
    SDL_Rect mainMenuButtonRect_;
    SDL_Rect dynamicMainMenuButtonRect_;
    SDL_Texture* gameOverBackgroundTexture_ = nullptr;
    SDL_Texture* menuImageTexture_ = nullptr; 
    SDL_Rect imagePanelRect_;                 
    SDL_Rect optionsPanelRect_;               
    SDL_Rect timeButton15Rect_;               
    SDL_Rect timeButton30Rect_;              
    SDL_Rect timeButton45Rect_;          
    SDL_Rect difficultyButtonEasyRect_;
    SDL_Rect difficultyButtonMediumRect_;
    SDL_Rect difficultyButtonHardRect_;
    AI::Difficulty selectedDifficulty_ = AI::Difficulty::MEDIUM;   
    bool isLoadingComplete_ = false;
    int loadingGameTime_ = 15; 
    AI::Difficulty loadingDifficulty_ = AI::Difficulty::EASY; 
    MoveResult currentMoveResult_;

    bool isDragging_ = false;
    int draggedRackIndex_ = -1;
    int selectedGameTime_ = 45;
    Tile draggedTile_;
    SDL_Point dragOffset_;
    SDL_Point originalDragPos_;
    int lastKnownPlayerId_ = -1;
    SDL_Point mousePos_ = {0, 0}; 
    Uint32 invalidMoveTimestamp_ = 0;
    int draggedBoardTileIndex_ = -1;
    SDL_Rect timerPanelRect_;
    std::vector<TempPlacedTile> currentMoveTiles_;
    std::vector<int> tilesToSwapIndices_;
    std::unordered_map<char, SDL_Texture*> tileTextureCache_;  
    std::unordered_map<std::string, SDL_Texture*> labelTextureCache_;
    std::map<char, SDL_Texture*> draggedTileTextureCache_;
    TTF_Font* fontDraggedTile_ = nullptr;
    TTF_Font* fontDraggedSmall_ = nullptr;

    bool init();
    void defineLayout();
    void handleEvents();
    void render();
    void updateGame();
    void update();
    void close();

    void renderBoard();
    void renderTile(const Tile& tile, int x, int y);
    void preRenderTileTextures();
    void preRenderLabelTextures();
    void validateCurrentMove(); 
    void renderLabel(const std::string& label, int x, int y, int w, int h);
    void renderFilledCircle(int centerX, int centerY, int radius, SDL_Color color);
    void renderRack();
    void renderSidebar();
    void renderButtons();
    void renderTimerPanel(const SDL_Rect& rect);
    std::string formatTime(Uint32 ms);
    void renderText(const std::string& text, int x, int y, TTF_Font* font, SDL_Color color);
    void renderText(const std::string& text, int containerX, int containerY, int containerW, int containerH, TTF_Font* font, SDL_Color color);
    void renderPlayerPanel(const Player* player, int playerId, const SDL_Rect& rect, bool isOpponent);
    void renderHistoryPanel(const SDL_Rect& rect);
    void renderTileBagPanel(const SDL_Rect& rect);
    void renderSuggestionPanel(const SDL_Rect& rect);
    void preRenderDraggedTileTextures();
    void renderLoading();
    void handleGameOverEvents();
    void handleLoadingEvents();
    void handleSwapSelectionEvents();
    void handleMenuEvents();
    void renderGameOver();
    void handleGameEvents();
    void renderMenu();
    void renderGame();
};