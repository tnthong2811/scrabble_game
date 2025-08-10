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
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    TTF_Font* fontNormal_ = nullptr;
    TTF_Font* fontSmall_ = nullptr;
    TTF_Font* fontBig_ = nullptr;
    Game& game_;
    bool running_ = false;

    SDL_Rect boardRect_;
    SDL_Rect rackRect_;
    SDL_Rect buttonsRect_;
    SDL_Rect sidebarRect_;

    bool isDragging_ = false;
    int draggedRackIndex_ = -1;
    Tile draggedTile_;
    SDL_Point dragOffset_;
    std::vector<TempPlacedTile> currentMoveTiles_;

    bool init();
    void defineLayout();
    void handleEvents();
    void render();
    void close();

    void renderBoard();
    void renderTile(const Tile& tile, int x, int y);
    void renderRack();
    void renderSidebar();
    void renderButtons();
    void renderText(const std::string& text, int x, int y, TTF_Font* font, SDL_Color color);
    
    // *** SỬA LỖI 2: Thêm tham số containerW vào khai báo ***
    void renderText(const std::string& text, int containerX, int containerY, int containerW, int containerH, TTF_Font* font, SDL_Color color);
};