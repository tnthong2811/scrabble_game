#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "core/Game.h"
#include "core/Board.h"
#include <vector>
#include <string>

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
        PLAYING,
        SELECTING_SWAP,
        GAME_OVER
    };

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    TTF_Font* fontNormal_ = nullptr;
    TTF_Font* fontTile_ = nullptr;
    TTF_Font* fontSmall_ = nullptr;
    TTF_Font* fontHistory_ = nullptr;
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
    SDL_Texture* gameOverBackgroundTexture_ = nullptr;
    SDL_Texture* menuImageTexture_ = nullptr; 
    SDL_Rect imagePanelRect_;                 
    SDL_Rect optionsPanelRect_;               
    SDL_Rect timeButton15Rect_;               
    SDL_Rect timeButton30Rect_;              
    SDL_Rect timeButton45Rect_;               

    bool isDragging_ = false;
    int draggedRackIndex_ = -1;
    int selectedGameTime_ = 45;
    Tile draggedTile_;
    SDL_Point dragOffset_;
    SDL_Point originalDragPos_;
    Uint32 invalidMoveTimestamp_ = 0;
    int draggedBoardTileIndex_ = -1;
    SDL_Rect timerPanelRect_;
    std::vector<TempPlacedTile> currentMoveTiles_;
    std::vector<int> tilesToSwapIndices_;

    bool init();
    void defineLayout();
    void handleEvents();
    void render();
    void updateGame();
    void update();
    void close();

    void renderBoard();
    void renderTile(const Tile& tile, int x, int y);
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
    void handleGameOverEvents();
    void handleSwapSelectionEvents();
    void handleMenuEvents();
    void renderGameOver();
    void handleGameEvents();
    void renderMenu();
    void renderGame();
};