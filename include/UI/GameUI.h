#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
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
        GAME_OVER
    };

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    TTF_Font* fontNormal_ = nullptr;
    TTF_Font* fontSmall_ = nullptr;
    TTF_Font* fontBig_ = nullptr;
    Game& game_;
    bool running_ = false;
    UIState currentState_ = UIState::MAIN_MENU;
    SDL_Rect playButtonRect_;
    SDL_Rect skipButtonRect_;
    SDL_Rect boardRect_;
    SDL_Rect rackRect_;
    SDL_Rect buttonsRect_;
    SDL_Rect sidebarRect_;
    SDL_Rect dynamicSubmitButtonRect_; 
    SDL_Rect dynamicPlayButtonRect_;
    SDL_Rect dynamicSkipButtonRect_;  

    bool isDragging_ = false;
    int draggedRackIndex_ = -1;
    Tile draggedTile_;
    SDL_Point dragOffset_;
    Uint32 invalidMoveTimestamp_ = 0;
    int draggedBoardTileIndex_ = -1;
    std::vector<TempPlacedTile> currentMoveTiles_;

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
    void renderText(const std::string& text, int x, int y, TTF_Font* font, SDL_Color color);
    void renderText(const std::string& text, int containerX, int containerY, int containerW, int containerH, TTF_Font* font, SDL_Color color);

    void handleMenuEvents();
    void handleGameEvents();
    void renderMenu();
    void renderGame();
};