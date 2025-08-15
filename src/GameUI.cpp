#include "UI/GameUI.h"
#include <iostream>
#include <string>
#include <algorithm>

const SDL_Color COLOR_BACKGROUND = { 34, 40, 49, 255 };    // Xám xanh đậm
const SDL_Color COLOR_BOARD_BG = { 20, 25, 30, 255 };      // Nền bàn cờ
const SDL_Color COLOR_NORMAL_CELL = { 205, 193, 180, 255 };
const SDL_Color COLOR_SIDEBAR = { 57, 62, 70, 255 };      // Xám vừa
const SDL_Color COLOR_TILE = { 251, 248, 239, 255 };      // Vàng kem
const SDL_Color COLOR_TEXT_DARK = { 50, 50, 50, 255 };     // Chữ tối
const SDL_Color COLOR_TEXT_LIGHT = { 238, 238, 238, 255 };  // Chữ sáng
const SDL_Color COLOR_BUTTON = { 0, 173, 181, 255 };       // Xanh mòng két

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 830;
const int BOARD_AREA_WIDTH = 800;
const int BOARD_X = 40;
const int BOARD_Y = 25;
const int BOARD_SIZE_PX = 640;
const int CELL_SIZE = BOARD_SIZE_PX / Board::SIZE;
const int TILE_SIZE = 40;

GameUI::GameUI(Game& gameLogic) : game_(gameLogic) { if (!init()) { /* Lỗi */ } }
GameUI::~GameUI() { close(); }

bool GameUI::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) return false;
    window_ = SDL_CreateWindow("Scrabble", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    fontNormal_ = TTF_OpenFont("assets/font/Pixel.ttf", 16);
    fontTile_ = TTF_OpenFont("assets/font/Pixel.ttf", 20);
    fontHistory_ = TTF_OpenFont("assets/font/Pixel.ttf", 14);
    fontSmall_ = TTF_OpenFont("assets/font/Pixel.ttf", 11);
    fontBig_ = TTF_OpenFont("assets/font/Pixel.ttf", 36);
    fontIVL_ = TTF_OpenFont("assets/font/Pixel.ttf", 50);
    fontTitle_ = TTF_OpenFont("assets/font/Pixel.ttf", 96);
    fontCoords_ = TTF_OpenFont("assets/font/Pixel.ttf", 12);
    fontDraggedTile_ = TTF_OpenFont("assets/font/Pixel.ttf", 22); 
    fontDraggedSmall_ = TTF_OpenFont("assets/font/Pixel.ttf", 12);

    menuImageTexture_ = IMG_LoadTexture(renderer_, "assets/image/menuimage.png");
    if (!menuImageTexture_) {
        std::cerr << "Lỗi không tải được assets/image/menu.png: " << IMG_GetError() << std::endl;
    }

    if (!window_ || !renderer_ || !fontNormal_ || !fontSmall_ || !fontBig_) return false;
    
    preRenderTileTextures();
    preRenderDraggedTileTextures();
    preRenderLabelTextures();

    defineLayout();
    return true;
}

void GameUI::defineLayout() {
    // --- BỐ CỤC MÀN HÌNH MENU ---
    // 1. Đặt kích thước cho các panel. Panel ảnh là hình vuông.
    const int IMAGE_PANEL_SIZE = 650;
    const int OPTIONS_PANEL_WIDTH = 500;

    // 2. Căn giữa toàn bộ khối menu theo chiều dọc
    int startY = (SCREEN_HEIGHT - IMAGE_PANEL_SIZE) / 2;
    if (startY < 0) startY = 0;

    // 3. Đặt vị trí cho panel ảnh 
    int imageStartX = 50; 
    imagePanelRect_ = { imageStartX, startY, IMAGE_PANEL_SIZE, IMAGE_PANEL_SIZE };

    // 4. Đặt panel tùy chọn ngay bên cạnh panel ảnh
    int optionsStartX = imageStartX + IMAGE_PANEL_SIZE + 50; // khoảng cách giữa 2 panel
    optionsPanelRect_ = { optionsStartX, startY, OPTIONS_PANEL_WIDTH, IMAGE_PANEL_SIZE };

    // Bố cục các nút bên trong panel tùy chọn
    int optionsCenterX = optionsPanelRect_.x + optionsPanelRect_.w / 2;
    int buttonWidth = 280;
    int menuButtonHeight = 55;
    int buttonSpacingMenu = 20;
    int groupSpacing = 160; // Khoảng cách giữa các nhóm tùy chọn

    // Vị trí các nút chọn độ khó 
    int difficultyButtonsTopY = optionsPanelRect_.y + 60;
    difficultyButtonEasyRect_ = { optionsCenterX - buttonWidth / 2, difficultyButtonsTopY, buttonWidth, menuButtonHeight };
    difficultyButtonMediumRect_ = { optionsCenterX - buttonWidth / 2, difficultyButtonEasyRect_.y + menuButtonHeight + buttonSpacingMenu, buttonWidth, menuButtonHeight };
    difficultyButtonHardRect_ = { optionsCenterX - buttonWidth / 2, difficultyButtonMediumRect_.y + menuButtonHeight + buttonSpacingMenu, buttonWidth, menuButtonHeight };
    // Vị trí các nút chọn thời gian 
    int timeButtonsTopY = difficultyButtonMediumRect_.y + menuButtonHeight + groupSpacing;
    timeButton15Rect_ = { optionsCenterX - buttonWidth / 2, timeButtonsTopY, buttonWidth, menuButtonHeight };
    timeButton30Rect_ = { optionsCenterX - buttonWidth / 2, timeButton15Rect_.y + menuButtonHeight + buttonSpacingMenu, buttonWidth, menuButtonHeight };
    timeButton45Rect_ = { optionsCenterX - buttonWidth / 2, timeButton30Rect_.y + menuButtonHeight + buttonSpacingMenu, buttonWidth, menuButtonHeight };
    
    // Vị trí nút Play 
    playButtonRect_ = { optionsCenterX - 125, optionsPanelRect_.y + optionsPanelRect_.h - 80, 250, 80 };
    // --- BỐ CỤC MÀN HÌNH CHƠI GAME ---
    const int COORDS_GUTTER = 25; 

    // 2. Dịch chuyển bàn cờ vào trong để có không gian
    boardRect_ = { BOARD_X + COORDS_GUTTER, BOARD_Y + COORDS_GUTTER, BOARD_SIZE_PX, BOARD_SIZE_PX };
    
    // 3. Tính toán chiều rộng và vị trí của các nút
    const int submitWidth = 130, skipWidth = 80, resetWidth = 90, swapWidth = 90, buttonSpacing = 15;
    const int inGameButtonHeight = 45;
    int totalButtonsWidth = submitWidth + skipWidth + resetWidth + swapWidth + (3 * buttonSpacing);

    // 4. Tính toán vị trí X để căn giữa khay và các nút với bàn cờ
    int boardCenterX = boardRect_.x + boardRect_.w / 2;
    int rackStartX = boardCenterX - totalButtonsWidth / 2;

    // 5. Định nghĩa khay rack (rackRect_) và các nút với vị trí X đã được căn giữa
    rackRect_ = { rackStartX, boardRect_.y + boardRect_.h + 15, totalButtonsWidth, 60 };
    buttonsRect_ = { rackRect_.x, rackRect_.y + rackRect_.h + 10, submitWidth, inGameButtonHeight };
    skipButtonRect_ = { buttonsRect_.x + buttonsRect_.w + buttonSpacing, buttonsRect_.y, skipWidth, inGameButtonHeight };
    resetButtonRect_ = { skipButtonRect_.x + skipButtonRect_.w + buttonSpacing, skipButtonRect_.y, resetWidth, inGameButtonHeight };
    swapButtonRect_ = { resetButtonRect_.x + resetButtonRect_.w + buttonSpacing, resetButtonRect_.y, swapWidth, inGameButtonHeight };

    // -- layout ---
    confirmSwapButtonRect_ = buttonsRect_; 
    sidebarRect_ = { BOARD_AREA_WIDTH, 0, SCREEN_WIDTH - BOARD_AREA_WIDTH, SCREEN_HEIGHT };
    
    // Bố cục của sidebar
    int sidebarPadding = 30;
    int panelSpacing = 15;
    
    playerInfoRect_ = { sidebarRect_.x + sidebarPadding, sidebarRect_.y + 40, sidebarRect_.w - 2 * sidebarPadding, 70 };
    opponentInfoRect_ = { playerInfoRect_.x, playerInfoRect_.y + playerInfoRect_.h + panelSpacing, playerInfoRect_.w, 70 };
    timerPanelRect_ = { playerInfoRect_.x, opponentInfoRect_.y + opponentInfoRect_.h + panelSpacing, playerInfoRect_.w, 60 };
    tileBagRect_ = { playerInfoRect_.x, timerPanelRect_.y + timerPanelRect_.h + panelSpacing, playerInfoRect_.w, 80 };
    turnHistoryRect_ = { playerInfoRect_.x, tileBagRect_.y + tileBagRect_.h + panelSpacing, playerInfoRect_.w, 180 };
    suggestionRect_ = { playerInfoRect_.x, turnHistoryRect_.y + turnHistoryRect_.h + panelSpacing, playerInfoRect_.w, 150 };
    
    // --- BỐ CỤC MÀN HÌNH KẾT THÚC ---
    const int gameOverButtonWidth = 200;
    const int gameOverButtonHeight = 60;
    const int gameOverButtonSpacing = 30;
    const int totalGameOverButtonsWidth = gameOverButtonWidth * 2 + gameOverButtonSpacing;

    int gameOverButtonsStartY = SCREEN_HEIGHT / 2 + 100;
    int gameOverButtonsStartX = (SCREEN_WIDTH - totalGameOverButtonsWidth) / 2;

    replayButtonRect_ = { gameOverButtonsStartX, gameOverButtonsStartY, gameOverButtonWidth, gameOverButtonHeight };
    mainMenuButtonRect_ = { replayButtonRect_.x + gameOverButtonWidth + gameOverButtonSpacing, gameOverButtonsStartY, gameOverButtonWidth, gameOverButtonHeight };

    // --- dynamic rects ---
    dynamicMainMenuButtonRect_ = mainMenuButtonRect_;
    dynamicSwapButtonRect_ = swapButtonRect_;
    dynamicConfirmSwapButtonRect_ = confirmSwapButtonRect_;
    dynamicSubmitButtonRect_ = buttonsRect_;
    dynamicSkipButtonRect_ = skipButtonRect_;
    dynamicResetButtonRect_ = resetButtonRect_;
    dynamicPlayButtonRect_ = playButtonRect_;
    dynamicReplayButtonRect_ = replayButtonRect_;
}

void GameUI::close() {
    TTF_CloseFont(fontNormal_);
    TTF_CloseFont(fontTile_);
    TTF_CloseFont(fontHistory_);
    TTF_CloseFont(fontSmall_);
    TTF_CloseFont(fontBig_);
    TTF_CloseFont(fontIVL_);
    TTF_CloseFont(fontTitle_);
    TTF_CloseFont(fontCoords_);
    TTF_CloseFont(fontDraggedTile_);
    TTF_CloseFont(fontDraggedSmall_);

    for (auto& p : draggedTileTextureCache_) SDL_DestroyTexture(p.second);
    for (auto& p : tileTextureCache_) SDL_DestroyTexture(p.second);
    for (auto& p : labelTextureCache_) SDL_DestroyTexture(p.second);

    if (menuImageTexture_) SDL_DestroyTexture(menuImageTexture_);
    if (gameOverBackgroundTexture_) SDL_DestroyTexture(gameOverBackgroundTexture_);

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void GameUI::run() {
    running_ = true;

    const double MS_PER_UPDATE = 8.333; // ~60 
    double previousTime = SDL_GetTicks();
    double lag = 0.0;
    Uint32 loadingStartTime = 0; 

    while (running_) {
        double currentTime = SDL_GetTicks();
        double elapsed = currentTime - previousTime;
        previousTime = currentTime;
        lag += elapsed;

        // Xử lý tất cả input của người dùng
        handleEvents();

        // Cập nhật logic game theo các bước thời gian cố định
        while (lag >= MS_PER_UPDATE) {
            update();
            if (currentState_ == UIState::LOADING) {
                if (loadingStartTime == 0) {
                    loadingStartTime = SDL_GetTicks();
                    std::cout << "Entered LOADING state at: " << loadingStartTime << " ms" << std::endl;
                    render();
                }

                if (!isLoadingComplete_) {
                    game_.startNewGame(1, loadingGameTime_, loadingDifficulty_);
                    isLoadingComplete_ = true;
                    std::cout << "Game initialized at: " << SDL_GetTicks() << " ms" << std::endl;
                }

                if (SDL_GetTicks() - loadingStartTime >= 0) {
                    currentState_ = UIState::PLAYING;
                    loadingStartTime = 0;
                    std::cout << "Switching to PLAYING state at: " << SDL_GetTicks() << " ms" << std::endl;
                }
            } else if (currentState_ == UIState::PLAYING) {
                updateGame();
                game_.updateTimers();
            }
            lag -= MS_PER_UPDATE;
        }
        
        render();
    }
}

void GameUI::update() {
    if (currentState_ == UIState::PLAYING && game_.getState() == Game::State::GAME_OVER) {
        if (gameOverBackgroundTexture_) {
            SDL_DestroyTexture(gameOverBackgroundTexture_);
            gameOverBackgroundTexture_ = nullptr;
        }
        gameOverBackgroundTexture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, 
                                                       SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
        SDL_SetRenderTarget(renderer_, gameOverBackgroundTexture_);
        renderGame();
        SDL_SetRenderTarget(renderer_, NULL);
        currentState_ = UIState::GAME_OVER;
    }

    int currentPlayerId = game_.getCurrentPlayerId();
    if (lastKnownPlayerId_ == 0 && currentPlayerId != 0) {
        if (!currentMoveTiles_.empty()) {
            currentMoveTiles_.clear();
            validateCurrentMove(); 
        }
    }
    lastKnownPlayerId_ = currentPlayerId;

    if (currentState_ == UIState::GAME_OVER) {
        const float scale = 1.1f;
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Hover nút Replay
        if (mouseX >= replayButtonRect_.x && mouseX < replayButtonRect_.x + replayButtonRect_.w &&
            mouseY >= replayButtonRect_.y && mouseY < replayButtonRect_.y + replayButtonRect_.h) {
            dynamicReplayButtonRect_.w = replayButtonRect_.w * scale;
            dynamicReplayButtonRect_.h = replayButtonRect_.h * scale;
            dynamicReplayButtonRect_.x = replayButtonRect_.x - (dynamicReplayButtonRect_.w - replayButtonRect_.w) / 2;
            dynamicReplayButtonRect_.y = replayButtonRect_.y - (dynamicReplayButtonRect_.h - replayButtonRect_.h) / 2;
        } else {
            dynamicReplayButtonRect_ = replayButtonRect_;
        }

        // Hover nút Main Menu
        if (mouseX >= mainMenuButtonRect_.x && mouseX < mainMenuButtonRect_.x + mainMenuButtonRect_.w &&
            mouseY >= mainMenuButtonRect_.y && mouseY < mainMenuButtonRect_.y + mainMenuButtonRect_.h) {
            dynamicMainMenuButtonRect_.w = mainMenuButtonRect_.w * scale;
            dynamicMainMenuButtonRect_.h = mainMenuButtonRect_.h * scale;
            dynamicMainMenuButtonRect_.x = mainMenuButtonRect_.x - (dynamicMainMenuButtonRect_.w - mainMenuButtonRect_.w) / 2;
            dynamicMainMenuButtonRect_.y = mainMenuButtonRect_.y - (dynamicMainMenuButtonRect_.h - mainMenuButtonRect_.h) / 2;
        } else {
            dynamicMainMenuButtonRect_ = mainMenuButtonRect_;
        }
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    const float scale = 1.1f;
    
    if (currentState_ == UIState::MAIN_MENU) {
        if (mouseX >= playButtonRect_.x && mouseX < playButtonRect_.x + playButtonRect_.w &&
            mouseY >= playButtonRect_.y && mouseY < playButtonRect_.y + playButtonRect_.h) {
            dynamicPlayButtonRect_.w = playButtonRect_.w * scale;
            dynamicPlayButtonRect_.h = playButtonRect_.h * scale;
            dynamicPlayButtonRect_.x = playButtonRect_.x - (dynamicPlayButtonRect_.w - playButtonRect_.w) / 2;
            dynamicPlayButtonRect_.y = playButtonRect_.y - (dynamicPlayButtonRect_.h - playButtonRect_.h) / 2;
        } else {
            dynamicPlayButtonRect_ = playButtonRect_;
        }
    }

        if (currentState_ == UIState::PLAYING || currentState_ == UIState::SELECTING_SWAP) {
        if (mouseX >= buttonsRect_.x && mouseX < buttonsRect_.x + buttonsRect_.w &&
            mouseY >= buttonsRect_.y && mouseY < buttonsRect_.y + buttonsRect_.h) {
            dynamicSubmitButtonRect_.w = buttonsRect_.w * scale;
            dynamicSubmitButtonRect_.h = buttonsRect_.h * scale;
            dynamicSubmitButtonRect_.x = buttonsRect_.x - (dynamicSubmitButtonRect_.w - buttonsRect_.w) / 2;
            dynamicSubmitButtonRect_.y = buttonsRect_.y - (dynamicSubmitButtonRect_.h - buttonsRect_.h) / 2;
        } else {
            dynamicSubmitButtonRect_ = buttonsRect_;
        }

        if (mouseX >= skipButtonRect_.x && mouseX < skipButtonRect_.x + skipButtonRect_.w &&
            mouseY >= skipButtonRect_.y && mouseY < skipButtonRect_.y + skipButtonRect_.h) {
            dynamicSkipButtonRect_.w = skipButtonRect_.w * scale;
            dynamicSkipButtonRect_.h = skipButtonRect_.h * scale;
            dynamicSkipButtonRect_.x = skipButtonRect_.x - (dynamicSkipButtonRect_.w - skipButtonRect_.w) / 2;
            dynamicSkipButtonRect_.y = skipButtonRect_.y - (dynamicSkipButtonRect_.h - skipButtonRect_.h) / 2;
        } else {
            dynamicSkipButtonRect_ = skipButtonRect_;
        }

        if (mouseX >= resetButtonRect_.x && mouseX < resetButtonRect_.x + resetButtonRect_.w &&
            mouseY >= resetButtonRect_.y && mouseY < resetButtonRect_.y + resetButtonRect_.h) {
            dynamicResetButtonRect_.w = resetButtonRect_.w * scale;
            dynamicResetButtonRect_.h = resetButtonRect_.h * scale;
            dynamicResetButtonRect_.x = resetButtonRect_.x - (dynamicResetButtonRect_.w - resetButtonRect_.w) / 2;
            dynamicResetButtonRect_.y = resetButtonRect_.y - (dynamicResetButtonRect_.h - resetButtonRect_.h) / 2;
        } else {
            dynamicResetButtonRect_ = resetButtonRect_;
        }

        if (mouseX >= swapButtonRect_.x && mouseX < swapButtonRect_.x + swapButtonRect_.w &&
            mouseY >= swapButtonRect_.y && mouseY < swapButtonRect_.y + swapButtonRect_.h) {
            dynamicSwapButtonRect_.w = swapButtonRect_.w * scale;
            dynamicSwapButtonRect_.h = swapButtonRect_.h * scale;
            dynamicSwapButtonRect_.x = swapButtonRect_.x - (dynamicSwapButtonRect_.w - swapButtonRect_.w) / 2;
            dynamicSwapButtonRect_.y = swapButtonRect_.y - (dynamicSwapButtonRect_.h - swapButtonRect_.h) / 2;
        } else {
            dynamicSwapButtonRect_ = swapButtonRect_;
        }
        
        if (currentState_ == UIState::SELECTING_SWAP) {
            if (mouseX >= confirmSwapButtonRect_.x && mouseX < confirmSwapButtonRect_.x + confirmSwapButtonRect_.w &&
                mouseY >= confirmSwapButtonRect_.y && mouseY < confirmSwapButtonRect_.y + confirmSwapButtonRect_.h) {
                dynamicConfirmSwapButtonRect_.w = confirmSwapButtonRect_.w * scale;
                dynamicConfirmSwapButtonRect_.h = confirmSwapButtonRect_.h * scale;
                dynamicConfirmSwapButtonRect_.x = confirmSwapButtonRect_.x - (dynamicConfirmSwapButtonRect_.w - confirmSwapButtonRect_.w) / 2;
                dynamicConfirmSwapButtonRect_.y = confirmSwapButtonRect_.y - (dynamicConfirmSwapButtonRect_.h - confirmSwapButtonRect_.h) / 2;
            } else {
                dynamicConfirmSwapButtonRect_ = confirmSwapButtonRect_;
            }
        }
    }
}

void GameUI::updateGame() {
    if (currentState_ == UIState::PLAYING) {
        if (game_.getState() == Game::State::PLAYING && game_.getCurrentPlayerId() != 0) {
            game_.update();
        }
    }
}

void GameUI::handleGameEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            running_ = false;
        }

        if (game_.getState() != Game::State::PLAYING || game_.getCurrentPlayerId() != 0) {
            continue;
        }

        Player* player = game_.getPlayer(0);
        if (!player) continue;

        // Cập nhật vị trí chuột khi di chuyển
        if (e.type == SDL_MOUSEMOTION) {
            mousePos_.x = e.motion.x;
            mousePos_.y = e.motion.y;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (isDragging_) continue;
            const int RACK_CAPACITY = 7;
            const int TILE_SPACING = 8;
            int totalTileWidth = RACK_CAPACITY * TILE_SIZE + (RACK_CAPACITY - 1) * TILE_SPACING;
            int startX = rackRect_.x + (rackRect_.w - totalTileWidth) / 2;
            int startY = rackRect_.y + (rackRect_.h - TILE_SIZE) / 2;

            int mouseX = e.button.x;
            int mouseY = e.button.y;

            // Kiểm tra nút Reset
            if (mouseX >= resetButtonRect_.x && mouseX < resetButtonRect_.x + resetButtonRect_.w &&
                mouseY >= resetButtonRect_.y && mouseY < resetButtonRect_.y + resetButtonRect_.h) {
                currentMoveTiles_.clear();
                validateCurrentMove();
                continue;
            }
            // Kiểm tra nút Skip
            if (mouseX >= skipButtonRect_.x && mouseX < skipButtonRect_.x + skipButtonRect_.w &&
                mouseY >= skipButtonRect_.y && mouseY < skipButtonRect_.y + skipButtonRect_.h) {
                currentMoveTiles_.clear();
                game_.passTurn(0);
                continue;
            }
            // Kiểm tra nút Submit
            if (mouseX >= buttonsRect_.x && mouseX < buttonsRect_.x + buttonsRect_.w && 
                mouseY >= buttonsRect_.y && mouseY < buttonsRect_.y + buttonsRect_.h) {
                if (!currentMoveTiles_.empty()) {
                    std::sort(currentMoveTiles_.begin(), currentMoveTiles_.end(), [](const auto& a, const auto& b) {
                        if (a.boardRow != b.boardRow) return a.boardRow < b.boardRow;
                        return a.boardCol < b.boardCol;
                    });
                    bool isHorizontal = (currentMoveTiles_.size() > 1) ? (currentMoveTiles_[0].boardRow == currentMoveTiles_[1].boardRow) : true;
                    if (currentMoveTiles_.size() == 1) {
                        int r = currentMoveTiles_[0].boardRow; int c = currentMoveTiles_[0].boardCol;
                        bool hNeighbor = game_.getBoard().hasTile(r, c - 1) || game_.getBoard().hasTile(r, c + 1);
                        if (!hNeighbor && (game_.getBoard().hasTile(r - 1, c) || game_.getBoard().hasTile(r + 1, c))) {
                            isHorizontal = false;
                        }
                    }
                    Board tempBoard = game_.getBoard();
                    for(const auto& placedTile : currentMoveTiles_) {
                        tempBoard.placeTileForAI(placedTile.boardRow, placedTile.boardCol, placedTile.tile);
                    }
                    int startRow = currentMoveTiles_.front().boardRow; int startCol = currentMoveTiles_.front().boardCol;
                    int finalStartRow = startRow; int finalStartCol = startCol;
                    if (isHorizontal) {
                        while (finalStartCol > 0 && tempBoard.hasTile(finalStartRow, finalStartCol - 1)) finalStartCol--;
                    } else {
                        while (finalStartRow > 0 && tempBoard.hasTile(finalStartRow - 1, finalStartCol)) finalStartRow--;
                    }
                    std::string fullWord = tempBoard.getWordAt(finalStartRow, finalStartCol, isHorizontal);
                    std::string wordFromRack;
                    for(const auto& pTile : currentMoveTiles_) wordFromRack += pTile.tile.getLetter();
                    bool success = game_.playWord(0, wordFromRack, fullWord, finalStartRow, finalStartCol, isHorizontal);
                    currentMoveTiles_.clear();
                    if (!success) {
                        invalidMoveTimestamp_ = SDL_GetTicks();
                    }
                }
                continue;
            }
            // Kiểm tra nút Swap
            if (mouseX >= swapButtonRect_.x && mouseX < swapButtonRect_.x + swapButtonRect_.w &&
                mouseY >= swapButtonRect_.y && mouseY < swapButtonRect_.y + swapButtonRect_.h) {                
                currentMoveTiles_.clear();
                tilesToSwapIndices_.clear();
                currentState_ = UIState::SELECTING_SWAP;
                continue;
            } 

            // Kiểm tra tile trên bàn cờ
            for (size_t i = 0; i < currentMoveTiles_.size(); ++i) {
                const auto& placedTile = currentMoveTiles_[i];
                SDL_Rect tileRect = { boardRect_.x + placedTile.boardCol * CELL_SIZE, boardRect_.y + placedTile.boardRow * CELL_SIZE, TILE_SIZE, TILE_SIZE };
                if (mouseX >= tileRect.x && mouseX < tileRect.x + tileRect.w && mouseY >= tileRect.y && mouseY < tileRect.y + tileRect.h) {
                    isDragging_ = true;
                    draggedTile_ = placedTile.tile;
                    draggedRackIndex_ = placedTile.originalRackIndex;
                    draggedBoardTileIndex_ = i;
                    originalDragPos_ = {placedTile.boardCol, placedTile.boardRow};
                    dragOffset_ = { mouseX - tileRect.x, mouseY - tileRect.y };
                    currentMoveTiles_.erase(currentMoveTiles_.begin() + i);
                    mousePos_ = { mouseX, mouseY }; // Cập nhật vị trí chuột
                    break;
                }
            }

            // Kiểm tra tile trên rack
            for (size_t i = 0; i < player->getRack().size(); ++i) {
                bool alreadyPlaced = false;
                for(const auto& placedTile : currentMoveTiles_) {
                    if (placedTile.originalRackIndex == (int)i) { alreadyPlaced = true; break; }
                }
                if (alreadyPlaced) continue;
                
                SDL_Rect tileRect = { startX + (int)i * (TILE_SIZE + TILE_SPACING), startY, TILE_SIZE, TILE_SIZE };
                
                if (mouseX >= tileRect.x && mouseX < tileRect.x + tileRect.w && mouseY >= tileRect.y && mouseY < tileRect.y + tileRect.h) {
                    isDragging_ = true;
                    draggedRackIndex_ = i;
                    draggedTile_ = player->getRack()[i];
                    draggedBoardTileIndex_ = -1;
                    dragOffset_ = { mouseX - tileRect.x, mouseY - tileRect.y };
                    mousePos_ = { mouseX, mouseY }; // Cập nhật vị trí chuột
                    break;
                }
            }
        } 
        else if (e.type == SDL_MOUSEBUTTONUP) {
            if (isDragging_) {
                if (draggedTile_.isBlank()) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    if (mouseX >= boardRect_.x && mouseX < boardRect_.x + boardRect_.w &&
                        mouseY >= boardRect_.y && mouseY < boardRect_.y + boardRect_.h)
                    {
                        int col = (mouseX - boardRect_.x) / CELL_SIZE;
                        int row = (mouseY - boardRect_.y) / CELL_SIZE;

                        // Chỉ hiện bảng chọn nếu thả vào ô trống
                        if (!game_.getBoard().hasTile(row, col) && !isTileTemporarilyPlacedAt(row, col)) {
                            // Lưu lại bối cảnh và chuyển sang trạng thái chọn chữ
                            blankPlacementRow_ = row;
                            blankPlacementCol_ = col;
                            blankOriginalRackIndex_ = draggedRackIndex_;
                            currentState_ = UIState::CHOOSING_BLANK_LETTER;
                            
                            isDragging_ = false; // Dừng kéo
                            return; // Thoát khỏi hàm để không chạy logic bên dưới
                        }
                    }
                }

                bool dropped_successfully = false;
                if (mousePos_.x >= boardRect_.x && mousePos_.x < boardRect_.x + boardRect_.w && 
                    mousePos_.y >= boardRect_.y && mousePos_.y < boardRect_.y + boardRect_.h) {
                    int col = (mousePos_.x - boardRect_.x) / CELL_SIZE;
                    int row = (mousePos_.y - boardRect_.y) / CELL_SIZE;
                    bool isOccupied = game_.getBoard().hasTile(row, col);
                    for(const auto& placedTile : currentMoveTiles_) {
                        if (placedTile.boardRow == row && placedTile.boardCol == col) { isOccupied = true; break; }
                    }
                    if (!isOccupied) {
                        currentMoveTiles_.push_back({draggedTile_, row, col, draggedRackIndex_});
                        dropped_successfully = true;
                    }
                }
                if (!dropped_successfully && mousePos_.x >= rackRect_.x && mousePos_.x < rackRect_.x + rackRect_.w && 
                    mousePos_.y >= rackRect_.y && mousePos_.y < rackRect_.y + rackRect_.h) {
                    dropped_successfully = true;
                }
                if (!dropped_successfully && draggedBoardTileIndex_ != -1) {
                    currentMoveTiles_.insert(currentMoveTiles_.begin() + draggedBoardTileIndex_, 
                                            {draggedTile_, originalDragPos_.y, originalDragPos_.x, draggedRackIndex_});
                }
                isDragging_ = false;
                draggedRackIndex_ = -1;
                draggedBoardTileIndex_ = -1;
                validateCurrentMove();
            }
        }
    }
}

void GameUI::renderGame() {
    SDL_SetRenderDrawColor(renderer_, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, 255);
    SDL_RenderClear(renderer_);

    renderBoard();
    renderSidebar();
    renderButtons();
    renderRack();

    if (isDragging_) {
        // Tính toán ô đang được chuột di chuyển qua
        bool isOverBoard = (mousePos_.x >= boardRect_.x && mousePos_.x < boardRect_.x + boardRect_.w &&
                            mousePos_.y >= boardRect_.y && mousePos_.y < boardRect_.y + boardRect_.h);
        if (isOverBoard) {
            int col = (mousePos_.x - boardRect_.x) / CELL_SIZE;
            int row = (mousePos_.y - boardRect_.y) / CELL_SIZE;
            bool isOccupied = game_.getBoard().hasTile(row, col);
            for (const auto& placedTile : currentMoveTiles_) {
                if (placedTile.boardRow == row && placedTile.boardCol == col) {
                    isOccupied = true;
                    break;
                }
            }

            // Vẽ hiệu ứng mờ nếu ô trống
            if (!isOccupied) {
                SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer_, 0, 173, 181, 100); // Xanh lam nhạt, alpha 100
                SDL_Rect highlightRect = { boardRect_.x + col * CELL_SIZE + 1, boardRect_.y + row * CELL_SIZE + 1, CELL_SIZE - 2, CELL_SIZE - 2 };
                SDL_RenderFillRect(renderer_, &highlightRect);
                SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);
            }
        }

        const float scale = 1.1f;
        int draggedSize = static_cast<int>(TILE_SIZE * scale);
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 100);
        SDL_Rect shadowRect = { mousePos_.x - dragOffset_.x + 3, mousePos_.y - dragOffset_.y + 3, draggedSize, draggedSize };
        SDL_RenderFillRect(renderer_, &shadowRect);
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);

        SDL_Rect tileRect = { mousePos_.x - dragOffset_.x, mousePos_.y - dragOffset_.y, draggedSize, draggedSize };
        char key = draggedTile_.isBlank() ? '?' : draggedTile_.getLetter();
        auto it = draggedTileTextureCache_.find(key);
        if (it != draggedTileTextureCache_.end()) {
            SDL_RenderCopy(renderer_, it->second, NULL, &tileRect);
        } else {
            SDL_SetRenderDrawColor(renderer_, COLOR_TILE.r, COLOR_TILE.g, COLOR_TILE.b, 255);
            SDL_RenderFillRect(renderer_, &tileRect);
            SDL_SetRenderDrawColor(renderer_, 80, 50, 20, 255);
            SDL_RenderDrawRect(renderer_, &tileRect);
        }
    }

    if (invalidMoveTimestamp_ != 0) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - invalidMoveTimestamp_ < 2000) {
            SDL_Color red = {255, 0, 0, 255};
            int x = boardRect_.x;
            int y = boardRect_.y;
            int w = boardRect_.w;
            int h = boardRect_.h;

            int textW, textH;
            TTF_SizeText(fontIVL_, "INVALID MOVE", &textW, &textH);
            int padding = 10;
            SDL_Rect backgroundRect = {
                x + (w - textW - padding * 2) / 2, 
                y + (h - textH - padding * 2) / 2, 
                textW + padding * 2,              
                textH + padding * 2               
            };

            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer_, &backgroundRect);
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);

            renderText("INVALID MOVE", x, y, w, h, fontIVL_, red);
        } else {
            invalidMoveTimestamp_ = 0;
        }
    }
}

void GameUI::handleSwapSelectionEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            running_ = false;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            Player* player = game_.getPlayer(0);
            if (!player) continue;

            // Xử lý click nút CONFIRM SWAP
            if (mouseX >= confirmSwapButtonRect_.x && mouseX < confirmSwapButtonRect_.x + confirmSwapButtonRect_.w &&
                mouseY >= confirmSwapButtonRect_.y && mouseY < confirmSwapButtonRect_.y + confirmSwapButtonRect_.h) {
                
                if (!tilesToSwapIndices_.empty()) {
                    std::vector<char> lettersToSwap;
                    const auto& rack = player->getRack();
                    for (int index : tilesToSwapIndices_) {
                        lettersToSwap.push_back(rack[index].getLetter());
                    }
                    game_.swapTiles(0, lettersToSwap);
                }
                tilesToSwapIndices_.clear();
                currentState_ = UIState::PLAYING;
                continue;
            }

            // Xử lý click nút CANCEL (dùng nút RESET)
            if (mouseX >= resetButtonRect_.x && mouseX < resetButtonRect_.x + resetButtonRect_.w &&
                mouseY >= resetButtonRect_.y && mouseY < resetButtonRect_.y + resetButtonRect_.h) {
                
                tilesToSwapIndices_.clear();
                currentState_ = UIState::PLAYING;
                continue;
            }

            const int RACK_CAPACITY = 7;
            const int TILE_SPACING = 8;
            int totalTileWidth = RACK_CAPACITY * TILE_SIZE + (RACK_CAPACITY - 1) * TILE_SPACING;
            int startX = rackRect_.x + (rackRect_.w - totalTileWidth) / 2;
            int startY = rackRect_.y + (rackRect_.h - TILE_SIZE) / 2;

            // Xử lý click chọn/bỏ chọn tile trên khay
            const auto& rack = player->getRack();
            for (size_t i = 0; i < rack.size(); ++i) {
                SDL_Rect tileRect = { startX + (int)i * (TILE_SIZE + TILE_SPACING), startY, TILE_SIZE, TILE_SIZE };

                if (mouseX >= tileRect.x && mouseX < tileRect.x + tileRect.w &&
                    mouseY >= tileRect.y && mouseY < tileRect.y + tileRect.h) {
                    
                    auto it = std::find(tilesToSwapIndices_.begin(), tilesToSwapIndices_.end(), i);
                    if (it != tilesToSwapIndices_.end()) {
                        tilesToSwapIndices_.erase(it);
                    } else {
                        tilesToSwapIndices_.push_back(i);
                    }
                }
            }
        }
    }
}

void GameUI::handleMenuEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            running_ = false;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            // Xử lý click nút Easy
            if (mouseX >= difficultyButtonEasyRect_.x && mouseX < difficultyButtonEasyRect_.x + difficultyButtonEasyRect_.w &&
                mouseY >= difficultyButtonEasyRect_.y && mouseY < difficultyButtonEasyRect_.y + difficultyButtonEasyRect_.h) {
                selectedDifficulty_ = AI::Difficulty::EASY;
            }
            // Xử lý click nút Medium
            else if (mouseX >= difficultyButtonMediumRect_.x && mouseX < difficultyButtonMediumRect_.x + difficultyButtonMediumRect_.w &&
                     mouseY >= difficultyButtonMediumRect_.y && mouseY < difficultyButtonMediumRect_.y + difficultyButtonMediumRect_.h) {
                selectedDifficulty_ = AI::Difficulty::MEDIUM;
            }
            //Xử lý click nút Hard
            else if (mouseX >= difficultyButtonHardRect_.x && mouseX < difficultyButtonHardRect_.x + difficultyButtonHardRect_.w &&
                     mouseY >= difficultyButtonHardRect_.y && mouseY < difficultyButtonHardRect_.y + difficultyButtonHardRect_.h) {
                selectedDifficulty_ = AI::Difficulty::HARD;
            }
            // Xử lý click nút 15 phút
            else if (mouseX >= timeButton15Rect_.x && mouseX < timeButton15Rect_.x + timeButton15Rect_.w &&
                mouseY >= timeButton15Rect_.y && mouseY < timeButton15Rect_.y + timeButton15Rect_.h) {
                selectedGameTime_ = 15;
            }
            // Xử lý click nút 30 phút
            else if (mouseX >= timeButton30Rect_.x && mouseX < timeButton30Rect_.x + timeButton30Rect_.w &&
                     mouseY >= timeButton30Rect_.y && mouseY < timeButton30Rect_.y + timeButton30Rect_.h) {
                selectedGameTime_ = 30;
            }
            // Xử lý click nút 45 phút
            else if (mouseX >= timeButton45Rect_.x && mouseX < timeButton45Rect_.x + timeButton45Rect_.w &&
                     mouseY >= timeButton45Rect_.y && mouseY < timeButton45Rect_.y + timeButton45Rect_.h) {
                selectedGameTime_ = 45;
            }
            // Xử lý click nút Play
            else if (mouseX >= playButtonRect_.x && mouseX < playButtonRect_.x + playButtonRect_.w &&
                     mouseY >= playButtonRect_.y && mouseY < playButtonRect_.y + playButtonRect_.h) {
                loadingGameTime_ = selectedGameTime_;
                loadingDifficulty_ = selectedDifficulty_;
                isLoadingComplete_ = false;
                currentState_ = UIState::LOADING;
            }
        }
    }
}

void GameUI::renderMenu() {
    // 1. Vẽ ảnh nền bên trái
    if (menuImageTexture_) {
        SDL_RenderCopy(renderer_, menuImageTexture_, NULL, &imagePanelRect_);
    } else {
        SDL_SetRenderDrawColor(renderer_, 30, 30, 30, 255);
        SDL_RenderFillRect(renderer_, &imagePanelRect_);
    }

    // 2. Vẽ panel tùy chọn bên phải
    SDL_SetRenderDrawColor(renderer_, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, 255);
    SDL_RenderFillRect(renderer_, &optionsPanelRect_);

    // --- VẼ NỘI DUNG PANEL TÙY CHỌN ---
    // Tiêu đề chọn độ khó
    renderText("Select Difficulty", optionsPanelRect_.x, optionsPanelRect_.y, optionsPanelRect_.w, 40, fontBig_, COLOR_TEXT_LIGHT);

    // Nút Easy
    SDL_SetRenderDrawColor(renderer_, (selectedDifficulty_ == AI::Difficulty::EASY) ? COLOR_BUTTON.r : 80, (selectedDifficulty_ == AI::Difficulty::EASY) ? COLOR_BUTTON.g : 80, (selectedDifficulty_ == AI::Difficulty::EASY) ? COLOR_BUTTON.b : 90, 255);
    SDL_RenderFillRect(renderer_, &difficultyButtonEasyRect_);
    renderText("Easy", difficultyButtonEasyRect_.x, difficultyButtonEasyRect_.y, difficultyButtonEasyRect_.w, difficultyButtonEasyRect_.h, fontNormal_, COLOR_TEXT_LIGHT);

    // Nút Medium
    SDL_SetRenderDrawColor(renderer_, (selectedDifficulty_ == AI::Difficulty::MEDIUM) ? COLOR_BUTTON.r : 80, (selectedDifficulty_ == AI::Difficulty::MEDIUM) ? COLOR_BUTTON.g : 80, (selectedDifficulty_ == AI::Difficulty::MEDIUM) ? COLOR_BUTTON.b : 90, 255);
    SDL_RenderFillRect(renderer_, &difficultyButtonMediumRect_);
    renderText("Medium", difficultyButtonMediumRect_.x, difficultyButtonMediumRect_.y, difficultyButtonMediumRect_.w, difficultyButtonMediumRect_.h, fontNormal_, COLOR_TEXT_LIGHT);

    SDL_SetRenderDrawColor(renderer_, (selectedDifficulty_ == AI::Difficulty::HARD) ? COLOR_BUTTON.r : 80, (selectedDifficulty_ == AI::Difficulty::HARD) ? COLOR_BUTTON.g : 80, (selectedDifficulty_ == AI::Difficulty::HARD) ? COLOR_BUTTON.b : 90, 255);
    SDL_RenderFillRect(renderer_, &difficultyButtonHardRect_);
    renderText("Hard", difficultyButtonHardRect_.x, difficultyButtonHardRect_.y, difficultyButtonHardRect_.w, difficultyButtonHardRect_.h, fontNormal_, COLOR_TEXT_LIGHT);

    // Tiêu đề chọn thời gian
    renderText("Select Game Time", optionsPanelRect_.x, optionsPanelRect_.y + 284, optionsPanelRect_.w, 40, fontBig_, COLOR_TEXT_LIGHT);
    
    // Nút 15 phút
    SDL_SetRenderDrawColor(renderer_, (selectedGameTime_ == 15) ? COLOR_BUTTON.r : 80, (selectedGameTime_ == 15) ? COLOR_BUTTON.g : 80, (selectedGameTime_ == 15) ? COLOR_BUTTON.b : 90, 255);
    SDL_RenderFillRect(renderer_, &timeButton15Rect_);
    renderText("15 Minutes", timeButton15Rect_.x, timeButton15Rect_.y, timeButton15Rect_.w, timeButton15Rect_.h, fontNormal_, COLOR_TEXT_LIGHT);

    // Nút 30 phút
    SDL_SetRenderDrawColor(renderer_, (selectedGameTime_ == 30) ? COLOR_BUTTON.r : 80, (selectedGameTime_ == 30) ? COLOR_BUTTON.g : 80, (selectedGameTime_ == 30) ? COLOR_BUTTON.b : 90, 255);
    SDL_RenderFillRect(renderer_, &timeButton30Rect_);
    renderText("30 Minutes", timeButton30Rect_.x, timeButton30Rect_.y, timeButton30Rect_.w, timeButton30Rect_.h, fontNormal_, COLOR_TEXT_LIGHT);

    // Nút 45 phút
    SDL_SetRenderDrawColor(renderer_, (selectedGameTime_ == 45) ? COLOR_BUTTON.r : 80, (selectedGameTime_ == 45) ? COLOR_BUTTON.g : 80, (selectedGameTime_ == 45) ? COLOR_BUTTON.b : 90, 255);
    SDL_RenderFillRect(renderer_, &timeButton45Rect_);
    renderText("45 Minutes", timeButton45Rect_.x, timeButton45Rect_.y, timeButton45Rect_.w, timeButton45Rect_.h, fontNormal_, COLOR_TEXT_LIGHT);

    // Vẽ nút Play
    SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
    SDL_RenderFillRect(renderer_, &dynamicPlayButtonRect_); 
    renderText("PLAY", dynamicPlayButtonRect_.x, dynamicPlayButtonRect_.y, dynamicPlayButtonRect_.w, dynamicPlayButtonRect_.h, fontBig_, COLOR_TEXT_LIGHT);
}

void GameUI::renderGameOver() {
    if (gameOverBackgroundTexture_) {
        SDL_RenderCopy(renderer_, gameOverBackgroundTexture_, NULL, NULL);
    }

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 180);
    SDL_Rect overlayRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer_, &overlayRect);
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);

    int winnerId = game_.getWinnerId();
    std::string message;
    SDL_Color color;

    if (winnerId == 0) {
        message = "YOU WIN!";
        color = {118, 255, 3, 255};
    } else {
        message = "GAME OVER";
        color = {211, 47, 47, 255};
    }

    renderText(message, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - 100, fontTitle_, color);

    SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
    SDL_RenderFillRect(renderer_, &dynamicReplayButtonRect_);
    renderText("REPLAY", dynamicReplayButtonRect_.x, dynamicReplayButtonRect_.y, 
               dynamicReplayButtonRect_.w, dynamicReplayButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);

    // Vẽ nút Main Menu
    SDL_SetRenderDrawColor(renderer_, 80, 80, 90, 255); // Màu xám cho nút phụ
    SDL_RenderFillRect(renderer_, &dynamicMainMenuButtonRect_);
    renderText("MAIN MENU", dynamicMainMenuButtonRect_.x, dynamicMainMenuButtonRect_.y, 
               dynamicMainMenuButtonRect_.w, dynamicMainMenuButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
}

void GameUI::handleGameOverEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            running_ = false;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Kiểm tra nếu nhấn vào nút Replay
            if (mouseX >= replayButtonRect_.x && mouseX < replayButtonRect_.x + replayButtonRect_.w &&
                mouseY >= replayButtonRect_.y && mouseY < replayButtonRect_.y + replayButtonRect_.h) {
                
                game_.startNewGame(1, selectedGameTime_, selectedDifficulty_);
                currentState_ = UIState::PLAYING;

                if (gameOverBackgroundTexture_) {
                    SDL_DestroyTexture(gameOverBackgroundTexture_);
                    gameOverBackgroundTexture_ = nullptr;
                }
            }
            // Kiểm tra nếu nhấn vào nút Main Menu
            else if (mouseX >= mainMenuButtonRect_.x && mouseX < mainMenuButtonRect_.x + mainMenuButtonRect_.w &&
                     mouseY >= mainMenuButtonRect_.y && mouseY < mainMenuButtonRect_.y + mainMenuButtonRect_.h) {
                currentState_ = UIState::MAIN_MENU;
                if (gameOverBackgroundTexture_) {
                    SDL_DestroyTexture(gameOverBackgroundTexture_);
                    gameOverBackgroundTexture_ = nullptr;
                }
            }
        }
    }
}

void GameUI::handleEvents() {
    if (currentState_ == UIState::MAIN_MENU) {
        handleMenuEvents();
    } else if (currentState_ == UIState::SELECTING_SWAP) {
        handleSwapSelectionEvents();
    } else if (currentState_ == UIState::GAME_OVER) {
        handleGameOverEvents();
    } else if (currentState_ == UIState::CHOOSING_BLANK_LETTER) { 
        handleBlankSelectionEvents();
    } else { 
        handleGameEvents();
    }
}

void GameUI::render() {
    SDL_SetRenderDrawColor(renderer_, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, 255);
    SDL_RenderClear(renderer_);

    if (currentState_ == UIState::MAIN_MENU) {
        renderMenu();
    } else if (currentState_ == UIState::GAME_OVER) {
        renderGameOver();
    } else if (currentState_ == UIState::LOADING) {
        renderLoading();
    } else {
        renderGame();
        if (currentState_ == UIState::CHOOSING_BLANK_LETTER) { 
            renderBlankSelectionPanel(); 
        }
    }
    SDL_RenderPresent(renderer_);
}

void GameUI::renderLabel(const std::string& label, int x, int y, int w, int h) {
    auto it = labelTextureCache_.find(label);
    if (it != labelTextureCache_.end()) {
        SDL_Rect dst = { x, y, w, h };
        SDL_RenderCopy(renderer_, it->second, NULL, &dst);
    } else {
        renderText(label, x, y, w, h, fontNormal_, COLOR_TEXT_DARK);
    }
}

void GameUI::renderBoard() {
    const int COORDS_GUTTER = 25; 

    // Vẽ số cho các cột (1-15, phía trên bàn cờ)
    for (int i = 0; i < Board::SIZE; ++i) {
        std::string numStr = std::to_string(i + 1);
        int x = boardRect_.x + i * CELL_SIZE;
        int y = boardRect_.y - COORDS_GUTTER;
        renderText(numStr, x, y, CELL_SIZE, COORDS_GUTTER, fontCoords_, COLOR_TEXT_LIGHT);
    }

    // Vẽ số cho các hàng (1-15, bên trái bàn cờ)
    for (int i = 0; i < Board::SIZE; ++i) {
        std::string numStr = std::to_string(i + 1);
        int x = boardRect_.x - COORDS_GUTTER;
        int y = boardRect_.y + i * CELL_SIZE;
        renderText(numStr, x, y, COORDS_GUTTER, CELL_SIZE, fontCoords_, COLOR_TEXT_LIGHT);
    }

    // 1. Vẽ nền cho bàn cờ
    SDL_SetRenderDrawColor(renderer_, COLOR_BOARD_BG.r, COLOR_BOARD_BG.g, COLOR_BOARD_BG.b, 255);
    SDL_RenderFillRect(renderer_, &boardRect_);

    const Board& board = game_.getBoard();
    SDL_Rect cellRect = { 0, 0, CELL_SIZE - 2, CELL_SIZE - 2 };

    // 2. Vòng lặp chính để vẽ 225 ô
    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            cellRect.x = boardRect_.x + c * CELL_SIZE + 1;
            cellRect.y = boardRect_.y + r * CELL_SIZE + 1;

            const auto& cell = board.getCell(r, c);

            // ƯU TIÊN 1: Nếu ô đã có chữ, hãy vẽ ô chữ đó.
            if (cell.hasTile()) {
                renderTile(cell.tile, cellRect.x, cellRect.y);
            } 
            // ƯU TIÊN 2: Nếu ô không có chữ, hãy vẽ ô đặc biệt (nếu có).
            else {
                std::string label = "";
                switch (cell.type) {
                    case CellType::TRIPLE_WORD:   SDL_SetRenderDrawColor(renderer_, 211, 47, 47, 255); label = "3W"; break;
                    case CellType::DOUBLE_WORD:  SDL_SetRenderDrawColor(renderer_, 255, 138, 128, 255); label = "2W"; break;
                    case CellType::TRIPLE_LETTER: SDL_SetRenderDrawColor(renderer_, 30, 136, 229, 255); label = "3L"; break;
                    case CellType::DOUBLE_LETTER: SDL_SetRenderDrawColor(renderer_, 129, 212, 250, 255); label = "2L"; break;
                    case CellType::NORMAL:        SDL_SetRenderDrawColor(renderer_, COLOR_NORMAL_CELL.r, COLOR_NORMAL_CELL.g, COLOR_NORMAL_CELL.b, 255); break;
                    case CellType::CENTER:
                        SDL_SetRenderDrawColor(renderer_, 255, 138, 128, 255);
                        SDL_RenderFillRect(renderer_, &cellRect);
                        // Vẽ ngôi sao
                        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 150);
                        int centerX = cellRect.x + cellRect.w / 2;
                        int centerY = cellRect.y + cellRect.h / 2;
                        int radius = cellRect.w / 4;
                        SDL_RenderDrawLine(renderer_, centerX, centerY - radius, centerX, centerY + radius);
                        SDL_RenderDrawLine(renderer_, centerX - radius, centerY, centerX + radius, centerY);
                        SDL_RenderDrawLine(renderer_, centerX - radius / 2, centerY - radius / 2, centerX + radius / 2, centerY + radius / 2);
                        SDL_RenderDrawLine(renderer_, centerX + radius / 2, centerY - radius / 2, centerX - radius / 2, centerY + radius / 2);
                        break;
                }

                if (cell.type != CellType::CENTER) {
                    SDL_RenderFillRect(renderer_, &cellRect);
                }
                if (!label.empty()) {
                    renderLabel(label, cellRect.x, cellRect.y, cellRect.w, cellRect.h);
                }
            }
        }
    }

    // 3. Cuối cùng, vẽ các ô chữ người chơi đang đặt tạm
    for (const auto& placedTile : currentMoveTiles_) {
        int x = boardRect_.x + placedTile.boardCol * CELL_SIZE + 1;
        int y = boardRect_.y + placedTile.boardRow * CELL_SIZE + 1;
        renderTile(placedTile.tile, x, y);

        // --- VIỀN XANH/ĐỎ ---
        SDL_Rect borderRect = { x-1, y-1, TILE_SIZE+2, TILE_SIZE+2 };
        if (currentMoveResult_.isValid) {
            SDL_SetRenderDrawColor(renderer_, 34, 177, 76, 255); // Xanh lá
        } else {
            SDL_SetRenderDrawColor(renderer_, 237, 28, 36, 255); // Đỏ
        }
        // Vẽ 2 lần cho viền dày hơn
        SDL_RenderDrawRect(renderer_, &borderRect);
        borderRect.x--; borderRect.y--; borderRect.w+=2; borderRect.h+=2;
        SDL_RenderDrawRect(renderer_, &borderRect);
    }

    // Vẽ điểm
    if (currentMoveResult_.isValid && !currentMoveTiles_.empty()) {
        // Lấy quân cờ ĐẦU TIÊN trong danh sách đã sắp xếp
        const auto& firstTile = currentMoveTiles_.front();
        
        // Tọa độ tâm của vòng tròn (góc trên bên trái của ô)
        int circleCenterX = boardRect_.x + firstTile.boardCol * CELL_SIZE;
        int circleCenterY = boardRect_.y + firstTile.boardRow * CELL_SIZE;
        int circleRadius = 10;

        // Vẽ nền tròn
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        renderFilledCircle(circleCenterX, circleCenterY, circleRadius, {34, 177, 76, 200});
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);
        
        // Chuẩn bị chuỗi điểm
        std::string scoreStr = std::to_string(currentMoveResult_.score);
        
        // Tạo một rect nhỏ xung quanh tâm để renderText có thể căn giữa
        SDL_Rect textRect = {circleCenterX - circleRadius, circleCenterY - circleRadius, circleRadius*2, circleRadius*2};
        renderText(scoreStr, textRect.x, textRect.y, textRect.w, textRect.h, fontSmall_, {255, 255, 255, 255}); // Màu trắng
    }
}

void GameUI::renderRack() {
    // 1. Vẽ một nền gỗ cho khay rack
    SDL_SetRenderDrawColor(renderer_, 205, 193, 180, 255);
    SDL_RenderFillRect(renderer_, &rackRect_);
    SDL_SetRenderDrawColor(renderer_, 205, 193, 180, 255); 
    SDL_RenderDrawRect(renderer_, &rackRect_);

    // 2. Vẽ các ô trống để đặt tile
    const int RACK_CAPACITY = 7; // Khay Scrabble có 7 ô
    const int TILE_SPACING = 8;
    int totalTileWidth = RACK_CAPACITY * TILE_SIZE + (RACK_CAPACITY - 1) * TILE_SPACING;
    int startX = rackRect_.x + (rackRect_.w - totalTileWidth) / 2;
    int startY = rackRect_.y + (rackRect_.h - TILE_SIZE) / 2;

    SDL_Rect slotRect = { 0, startY, TILE_SIZE, TILE_SIZE };
    SDL_SetRenderDrawColor(renderer_, 65, 40, 30, 150); // Màu rãnh tối hơn

    for (int i = 0; i < RACK_CAPACITY; ++i) {
        slotRect.x = startX + i * (TILE_SIZE + TILE_SPACING);
        SDL_RenderFillRect(renderer_, &slotRect);
    }

    // 3. Vẽ các tile của người chơi lên trên các ô trống
    Player* player = game_.getPlayer(game_.getCurrentPlayerId());
    if (!player) return;

    const auto& rack = player->getRack();
    for (size_t i = 0; i < rack.size(); ++i) {
        // Ẩn tile nếu nó đang được kéo hoặc đã được đặt tạm lên bàn cờ
        if (isDragging_ && (int)i == draggedRackIndex_) {
            continue;
        }
        bool alreadyPlaced = false;
        for(const auto& placedTile : currentMoveTiles_) {
            if (placedTile.originalRackIndex == (int)i) {
                alreadyPlaced = true;
                break;
            }
        }
        if (alreadyPlaced) {
            continue;
        }

        // Tính toán vị trí x để đặt tile vào đúng ô
        int x = startX + i * (TILE_SIZE + TILE_SPACING);
        int y = startY;

        // Xử lý highlight khi chọn đổi bài
        bool isSelectedForSwap = false;
        if (currentState_ == UIState::SELECTING_SWAP) {
            auto it = std::find(tilesToSwapIndices_.begin(), tilesToSwapIndices_.end(), i);
            if (it != tilesToSwapIndices_.end()) {
                isSelectedForSwap = true;
            }
        }

        if (isSelectedForSwap) {
            renderTile(rack[i], x, y);
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer_, 0, 173, 181, 100);
            SDL_Rect highlightRect = {x, y, TILE_SIZE, TILE_SIZE};
            SDL_RenderFillRect(renderer_, &highlightRect);
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);
        } else {
            renderTile(rack[i], x, y);
        }
    }
}

void GameUI::renderTile(const Tile& tile, int x, int y) {
    SDL_Rect tileRect = { x, y, TILE_SIZE, TILE_SIZE };
    char key = tile.isBlank() ? '?' : tile.getLetter();
    auto it = tileTextureCache_.find(key);
    if (it != tileTextureCache_.end()) {
        SDL_RenderCopy(renderer_, it->second, NULL, &tileRect);
    } else {
        SDL_Texture* tex = createTileTexture(tile);
        SDL_RenderCopy(renderer_, tex, NULL, &tileRect);
        SDL_DestroyTexture(tex);
    }

    // Chỉ vẽ chữ nếu là blank tile đã được gán chữ (không vẽ lại cho tile thông thường)
    if (tile.isBlank() && tile.getLetter() != ' ') {
        std::string letterStr(1, tile.getLetter());
        SDL_Color letterColor = {120, 120, 120, 255};  // Màu xám để phân biệt blank
        renderText(letterStr, tileRect.x, tileRect.y, tileRect.w, tileRect.h - 5, fontTile_, letterColor);
    }
}

void GameUI::renderTileBagPanel(const SDL_Rect& rect) {
    SDL_SetRenderDrawColor(renderer_, 40, 50, 60, 255);
    SDL_RenderFillRect(renderer_, &rect);
    renderText("Tile Bag", rect.x, rect.y + 10, rect.w, 20, fontNormal_, COLOR_TEXT_LIGHT);
    std::string remaining = "Remaining: " + std::to_string(game_.getTileBag().remainingTiles());
    renderText(remaining, rect.x, rect.y + 35, rect.w, 20, fontNormal_, COLOR_TEXT_LIGHT);
}

void GameUI::renderHistoryPanel(const SDL_Rect& rect) {
    SDL_SetRenderDrawColor(renderer_, 40, 50, 60, 255);
    SDL_RenderFillRect(renderer_, &rect);
    renderText("Turn History", rect.x, rect.y + 10, rect.w, 20, fontNormal_, COLOR_TEXT_LIGHT);
    const auto& history = game_.getTurnHistory();
    if (history.empty()) {
        return; 
    }
    int movesToShow = std::min(4, (int)history.size());
    int y_offset = 40; 
    for (int i = 0; i < movesToShow; ++i) {
        const auto& turn = history[history.size() - 1 - i];
        std::string text;
        if (turn.isPass) {
            text = turn.playerName + " passed.";
        } else if (turn.isSwap) { 
            text = turn.playerName + " swapped tiles.";
        }
        else {
            text = turn.playerName + " played '" + turn.word + "' for " + std::to_string(turn.score) + " pts.";
        }
        renderText(text, rect.x + 15, rect.y + y_offset, fontHistory_, COLOR_TEXT_LIGHT);
        y_offset += 20;
    }
}

void GameUI::renderPlayerPanel(const Player* player, int playerId, const SDL_Rect& rect, bool isOpponent) {
    // Tránh lỗi nếu con trỏ player không hợp lệ
    if (!player) return;

    // 1. Vẽ nền cho panel thông tin người chơi
    SDL_SetRenderDrawColor(renderer_, 40, 50, 60, 255); // Màu nền tối hơn một chút
    SDL_RenderFillRect(renderer_, &rect);

    // 2. Lấy thông tin tên và điểm số
    std::string name = player->getName();
    std::string score = "Score: " + std::to_string(player->getScore());

    // 3. Quyết định màu chữ: 
    // Nếu ID của người chơi hiện tại trùng với ID được truyền vào, dùng màu sáng (highlight).
    // Nếu không, dùng màu xám mờ.
    SDL_Color textColor = (game_.getCurrentPlayerId() == playerId) ? COLOR_TEXT_LIGHT : SDL_Color{150, 150, 150, 255};
    
    // 4. Vẽ tên và điểm số ra màn hình
    renderText(name, rect.x + 20, rect.y + 15, fontNormal_, textColor);
    renderText(score, rect.x + 20, rect.y + 40, fontNormal_, textColor);
}

void GameUI::renderSuggestionPanel(const SDL_Rect& rect) {
    SDL_SetRenderDrawColor(renderer_, 40, 50, 60, 255);
    SDL_RenderFillRect(renderer_, &rect);
    renderText("Suggestions", rect.x, rect.y + 10, rect.w, 20, fontNormal_, COLOR_TEXT_LIGHT);

    const auto& suggestions = game_.getSuggestions();

    if (suggestions.empty()) {
        renderText("No moves available.", rect.x, rect.y + 40, rect.w, 20, fontHistory_, {180, 180, 180, 255});
        return;
    }

    int y_offset = 40;
    for (const auto& play : suggestions) {
        if (y_offset > rect.h - 20) break; 

        Move move = play.getMove();
        std::string coords = "(" + std::to_string(move.getCol() + 1) + "," + std::to_string(move.getRow() + 1) + ")";
        std::string direction = (move.getDirection() == Move::Direction::HORIZONTAL) ? " (HORIZONTAL)" : " (VERTICAL)";
        
        std::string text = move.getWord() + " " + coords + direction;
        
        renderText(text, rect.x + 15, rect.y + y_offset, fontHistory_, COLOR_TEXT_LIGHT);
        y_offset += 20; 
    }
}

void GameUI::renderSidebar() {
    // Vẽ nền chính của sidebar
    SDL_SetRenderDrawColor(renderer_, COLOR_SIDEBAR.r, COLOR_SIDEBAR.g, COLOR_SIDEBAR.b, 255);
    SDL_RenderFillRect(renderer_, &sidebarRect_);

    Player* human_player = game_.getPlayer(0);
    Player* ai_player = game_.getPlayer(1);

    renderPlayerPanel(human_player, 0, playerInfoRect_, false); 
    renderPlayerPanel(ai_player, 1, opponentInfoRect_, true);
    renderTimerPanel(timerPanelRect_);
    renderTileBagPanel(tileBagRect_);
    renderHistoryPanel(turnHistoryRect_);
    renderSuggestionPanel(suggestionRect_);
}

void GameUI::renderButtons() {
    if(currentState_ == UIState::PLAYING) {
        SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
        SDL_RenderFillRect(renderer_, &dynamicSubmitButtonRect_); 
        renderText("SUBMIT", dynamicSubmitButtonRect_.x, dynamicSubmitButtonRect_.y, dynamicSubmitButtonRect_.w, dynamicSubmitButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
        SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
        SDL_RenderFillRect(renderer_, &dynamicSkipButtonRect_);
        renderText("SKIP", dynamicSkipButtonRect_.x, dynamicSkipButtonRect_.y, dynamicSkipButtonRect_.w, dynamicSkipButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
        SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
        SDL_RenderFillRect(renderer_, &dynamicResetButtonRect_);
        renderText("RESET", dynamicResetButtonRect_.x, dynamicResetButtonRect_.y, dynamicResetButtonRect_.w, dynamicResetButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
        SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
        SDL_RenderFillRect(renderer_, &dynamicSwapButtonRect_);
        renderText("SWAP", dynamicSwapButtonRect_.x, dynamicSwapButtonRect_.y, dynamicSwapButtonRect_.w, dynamicSwapButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
    } else if (currentState_ == UIState::SELECTING_SWAP) {
        // Vẽ nút CONFIRM (sử dụng vị trí của nút SUBMIT)
        SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
        SDL_RenderFillRect(renderer_, &dynamicConfirmSwapButtonRect_); 
        renderText("CONFIRM", dynamicConfirmSwapButtonRect_.x, dynamicConfirmSwapButtonRect_.y, dynamicConfirmSwapButtonRect_.w, dynamicConfirmSwapButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);

        // Vẽ nút CANCEL (sử dụng vị trí của nút RESET)
        SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
        SDL_RenderFillRect(renderer_, &dynamicResetButtonRect_);
        renderText("CANCEL", dynamicResetButtonRect_.x, dynamicResetButtonRect_.y, dynamicResetButtonRect_.w, dynamicResetButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
    }
}

void GameUI::renderText(const std::string& text, int x, int y, TTF_Font* font, SDL_Color color) {
    if (!font || text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer_, texture, NULL, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void GameUI::renderText(const std::string& text, int containerX, int containerY, int containerW, int containerH, TTF_Font* font, SDL_Color color) {
    if (!font || text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    int textW = surface->w;
    int textH = surface->h;
    int x = containerX + (containerW - textW) / 2;
    int y = containerY + (containerH - textH) / 2;
    SDL_Rect destRect = { x, y, textW, textH };
    SDL_RenderCopy(renderer_, texture, NULL, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

std::string GameUI::formatTime(Uint32 ms) {
    if (ms == 0) return "00:00";
    Uint32 totalSeconds = ms / 1000;
    Uint32 minutes = totalSeconds / 60;
    Uint32 seconds = totalSeconds % 60;

    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02u:%02u", minutes, seconds);
    return std::string(buffer);
}

void GameUI::renderTimerPanel(const SDL_Rect& rect) {
    // Vẽ nền
    SDL_SetRenderDrawColor(renderer_, 40, 50, 60, 255);
    SDL_RenderFillRect(renderer_, &rect);

    // Lấy và định dạng thời gian
    std::string totalTimeStr = "Game: " + formatTime(game_.getTotalTimeRemaining());
    std::string turnTimeStr = "Turn: " + formatTime(game_.getTurnTimeRemaining());

    // Vẽ thời gian
    renderText(totalTimeStr, rect.x + 15, rect.y + 10, fontNormal_, COLOR_TEXT_LIGHT);
    renderText(turnTimeStr, rect.x + 15, rect.y + 35, fontNormal_, COLOR_TEXT_LIGHT);
}

void GameUI::handleLoadingEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            running_ = false;
        }
    }
}

void GameUI::renderLoading() {
    std::cout << "Rendering LOADING screen..." << std::endl;
    SDL_SetRenderDrawColor(renderer_, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, 255);
    SDL_RenderClear(renderer_);

    SDL_Color loadingColor = {255, 255, 255, 255}; 
    renderText("LOADING...", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, fontTitle_, loadingColor);
}

SDL_Texture* GameUI::createTileTexture(const Tile& tile) {
    SDL_Texture* tex = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TILE_SIZE, TILE_SIZE);
    SDL_SetRenderTarget(renderer_, tex);
    SDL_SetRenderDrawColor(renderer_, COLOR_TILE.r, COLOR_TILE.g, COLOR_TILE.b, 255);
    SDL_RenderFillRect(renderer_, NULL);
    SDL_SetRenderDrawColor(renderer_, 80, 50, 20, 255);
    SDL_RenderDrawRect(renderer_, NULL);

    if (!tile.isBlank()) {
        std::string letter(1, tile.getLetter());
        SDL_Surface* letterSurf = TTF_RenderText_Blended(fontTile_, letter.c_str(), COLOR_TEXT_DARK);
        if (letterSurf) {
            SDL_Texture* letterTex = SDL_CreateTextureFromSurface(renderer_, letterSurf);
            int w = letterSurf->w;
            int h = letterSurf->h;
            SDL_Rect dst = { (TILE_SIZE - w) / 2, (TILE_SIZE - h) / 2 - 5, w, h };  // Center, y up 5px for balance
            SDL_RenderCopy(renderer_, letterTex, NULL, &dst);
            SDL_DestroyTexture(letterTex);
            SDL_FreeSurface(letterSurf);
        }

        std::string value = std::to_string(tile.getValue());
        SDL_Surface* valueSurf = TTF_RenderText_Blended(fontSmall_, value.c_str(), COLOR_TEXT_DARK);
        if (valueSurf) {
            SDL_Texture* valueTex = SDL_CreateTextureFromSurface(renderer_, valueSurf);
            int w = valueSurf->w;
            int h = valueSurf->h;
            SDL_Rect dst = { TILE_SIZE - w - 4, TILE_SIZE - h - 4, w, h };  // Bottom right, padding 4px
            SDL_RenderCopy(renderer_, valueTex, NULL, &dst);
            SDL_DestroyTexture(valueTex);
            SDL_FreeSurface(valueSurf);
        }
    }

    SDL_SetRenderTarget(renderer_, NULL);
    return tex;
}

SDL_Texture* GameUI::createLabelTexture(const std::string& label) {
    SDL_Texture* tex = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, CELL_SIZE - 2, CELL_SIZE - 2);
    SDL_SetRenderTarget(renderer_, tex);
    
    if (label == "3W") {
        SDL_SetRenderDrawColor(renderer_, 211, 47, 47, 255); 
    } else if (label == "2W") {
        SDL_SetRenderDrawColor(renderer_, 255, 138, 128, 255); 
    } else if (label == "3L") {
        SDL_SetRenderDrawColor(renderer_, 30, 136, 229, 255); 
    } else if (label == "2L") {
        SDL_SetRenderDrawColor(renderer_, 129, 212, 250, 255); 
    } else {
        SDL_SetRenderDrawColor(renderer_, COLOR_NORMAL_CELL.r, COLOR_NORMAL_CELL.g, COLOR_NORMAL_CELL.b, 255); // Màu mặc định
    }
    SDL_RenderClear(renderer_);

    SDL_Surface* surf = TTF_RenderText_Blended(fontNormal_, label.c_str(), COLOR_TEXT_DARK);
    if (surf) {
        SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer_, surf);
        int w = surf->w;
        int h = surf->h;
        SDL_Rect dst = { (CELL_SIZE - 2 - w) / 2, (CELL_SIZE - 2 - h) / 2, w, h }; // Căn giữa
        SDL_RenderCopy(renderer_, textTex, NULL, &dst);
        SDL_DestroyTexture(textTex);
        SDL_FreeSurface(surf);
    }

    SDL_SetRenderTarget(renderer_, NULL);
    return tex;
}

void GameUI::preRenderTileTextures() {
    for (char ch = 'A'; ch <= 'Z'; ++ch) {
        Tile tile(ch, false);
        tileTextureCache_[ch] = createTileTexture(tile);
    }
    Tile blank('?', true);
    tileTextureCache_['?'] = createTileTexture(blank);
}

void GameUI::preRenderLabelTextures() {
    std::vector<std::string> labels = {"3W", "2W", "3L", "2L"};
    for (auto& label : labels) {
        labelTextureCache_[label] = createLabelTexture(label);
    }
}

SDL_Texture* GameUI::createDraggedTileTexture(const Tile& tile) {
    const float scale = 1.1f;
    int draggedSize = static_cast<int>(TILE_SIZE * scale);
    SDL_Texture* tex = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, draggedSize, draggedSize);
    SDL_SetRenderTarget(renderer_, tex);
    SDL_SetRenderDrawColor(renderer_, COLOR_TILE.r, COLOR_TILE.g, COLOR_TILE.b, 255);
    SDL_RenderClear(renderer_);
    SDL_SetRenderDrawColor(renderer_, 80, 50, 20, 255);
    SDL_Rect fullRect = {0, 0, draggedSize, draggedSize};
    SDL_RenderDrawRect(renderer_, &fullRect);

    if (!tile.isBlank()) {
        std::string letter(1, tile.getLetter());
        SDL_Surface* letterSurf = TTF_RenderText_Blended(fontDraggedTile_, letter.c_str(), COLOR_TEXT_DARK);
        if (letterSurf) {
            SDL_Texture* letterTex = SDL_CreateTextureFromSurface(renderer_, letterSurf);
            int w = letterSurf->w;
            int h = letterSurf->h;
            SDL_Rect dst = { (draggedSize - w) / 2, (draggedSize - h) / 2 - static_cast<int>(5 * scale), w, h };
            SDL_RenderCopy(renderer_, letterTex, NULL, &dst);
            SDL_DestroyTexture(letterTex);
            SDL_FreeSurface(letterSurf);
        }

        std::string value = std::to_string(tile.getValue());
        SDL_Surface* valueSurf = TTF_RenderText_Blended(fontDraggedSmall_, value.c_str(), COLOR_TEXT_DARK);
        if (valueSurf) {
            SDL_Texture* valueTex = SDL_CreateTextureFromSurface(renderer_, valueSurf);
            int w = valueSurf->w;
            int h = valueSurf->h;
            SDL_Rect dst = { draggedSize - w - static_cast<int>(4 * scale), draggedSize - h - static_cast<int>(4 * scale), w, h };
            SDL_RenderCopy(renderer_, valueTex, NULL, &dst);
            SDL_DestroyTexture(valueTex);
            SDL_FreeSurface(valueSurf);
        }
    }

    SDL_SetRenderTarget(renderer_, NULL);
    return tex;
}

void GameUI::preRenderDraggedTileTextures() {
    for (char ch = 'A'; ch <= 'Z'; ++ch) {
        Tile tile(ch, false);
        draggedTileTextureCache_[ch] = createDraggedTileTexture(tile);
    }
    Tile blank('?', true);
    draggedTileTextureCache_['?'] = createDraggedTileTexture(blank);
}

void GameUI::validateCurrentMove() {
    if (currentMoveTiles_.empty()) {
        currentMoveResult_ = MoveResult();
        return;
    }

    std::sort(currentMoveTiles_.begin(), currentMoveTiles_.end(), [](const auto& a, const auto& b) {
        if (a.boardRow != b.boardRow) return a.boardRow < b.boardRow;
        return a.boardCol < b.boardCol;
    });

    bool isHorizontal = true;
    if (currentMoveTiles_.size() > 1) {
        isHorizontal = (currentMoveTiles_[0].boardRow == currentMoveTiles_[1].boardRow);
    } else {
        int r = currentMoveTiles_[0].boardRow;
        int c = currentMoveTiles_[0].boardCol;
        bool hNeighbor = game_.getBoard().hasTile(r, c - 1) || game_.getBoard().hasTile(r, c + 1);
        bool vNeighbor = game_.getBoard().hasTile(r - 1, c) || game_.getBoard().hasTile(r + 1, c);
        if (!hNeighbor && vNeighbor) {
            isHorizontal = false;
        }
    }
    
    Board tempBoard = game_.getBoard();
    for(const auto& placedTile : currentMoveTiles_) {
        tempBoard.placeTileForAI(placedTile.boardRow, placedTile.boardCol, placedTile.tile);
    }

    int startRow = currentMoveTiles_.front().boardRow;
    int startCol = currentMoveTiles_.front().boardCol;

    if (isHorizontal) {
        while (startCol > 0 && tempBoard.hasTile(startRow, startCol - 1)) startCol--;
    } else {
        while (startRow > 0 && tempBoard.hasTile(startRow - 1, startCol)) startRow--;
    }
    
    std::string fullWord = tempBoard.getWordAt(startRow, startCol, isHorizontal);
    
    Move move(fullWord, startRow, startCol, isHorizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL);
    
    Player* player = game_.getPlayer(0);
    if (player) {
        currentMoveResult_ = game_.getBoard().validateAndScoreMove(move, *player, game_.getDictionary());
    } else {
        currentMoveResult_ = MoveResult();
    }
}

void GameUI::renderFilledCircle(int centerX, int centerY, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // offset theo chiều ngang
            int dy = radius - h; // offset theo chiều dọc
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer_, centerX + dx, centerY + dy);
            }
        }
    }
}

void GameUI::renderBlankSelectionPanel() {
    // 1. Vẽ một lớp nền đen mờ
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer_, &overlay);
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);

    // 2. Vẽ panel chính
    SDL_Rect panelRect = { SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 150, 500, 300 };
    SDL_SetRenderDrawColor(renderer_, COLOR_SIDEBAR.r, COLOR_SIDEBAR.g, COLOR_SIDEBAR.b, 255);
    SDL_RenderFillRect(renderer_, &panelRect);
    SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
    SDL_RenderDrawRect(renderer_, &panelRect);

    // 3. Vẽ tiêu đề
    renderText("Choose a Letter", panelRect.x, panelRect.y + 20, panelRect.w, 40, fontBig_, COLOR_TEXT_LIGHT);

    // 4. Vẽ các nút chữ cái
    const int BUTTON_SIZE = 35;
    const int BUTTON_SPACING = 5;
    int startX = panelRect.x + (panelRect.w - (13 * (BUTTON_SIZE + BUTTON_SPACING))) / 2;
    int startY = panelRect.y + 100;

    for (int i = 0; i < 26; ++i) {
        char letter = 'A' + i;
        int row = i / 13;
        int col = i % 13;

        SDL_Rect buttonRect = { startX + col * (BUTTON_SIZE + BUTTON_SPACING), startY + row * (BUTTON_SIZE + BUTTON_SPACING + 10), BUTTON_SIZE, BUTTON_SIZE };
        SDL_SetRenderDrawColor(renderer_, 80, 80, 90, 255);
        SDL_RenderFillRect(renderer_, &buttonRect);
        renderText(std::string(1, letter), buttonRect.x, buttonRect.y, buttonRect.w, buttonRect.h, fontTile_, COLOR_TEXT_LIGHT);
    }
}

void GameUI::handleBlankSelectionEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            running_ = false;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Logic kiểm tra click vào nút chữ cái
            const int BUTTON_SIZE = 35;
            const int BUTTON_SPACING = 5;
            SDL_Rect panelRect = { SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 150, 500, 300 };
            int startX = panelRect.x + (panelRect.w - (13 * (BUTTON_SIZE + BUTTON_SPACING))) / 2;
            int startY = panelRect.y + 100;

            for (int i = 0; i < 26; ++i) {
                char letter = 'A' + i;
                int row = i / 13;
                int col = i % 13;
                SDL_Rect buttonRect = { startX + col * (BUTTON_SIZE + BUTTON_SPACING), startY + row * (BUTTON_SIZE + BUTTON_SPACING + 10), BUTTON_SIZE, BUTTON_SIZE };

                if (mouseX >= buttonRect.x && mouseX < buttonRect.x + buttonRect.w &&
                    mouseY >= buttonRect.y && mouseY < buttonRect.y + buttonRect.h) {
                    
                    // Người chơi đã chọn một chữ
                    Tile blankTile('?', 0, true); // Tạo quân trắng
                    blankTile.assignLetter(letter); // Gán chữ cho nó

                    // Thêm quân cờ đã được gán chữ vào danh sách đặt tạm
                    currentMoveTiles_.push_back({blankTile, blankPlacementRow_, blankPlacementCol_, blankOriginalRackIndex_});
                    
                    // Quay lại trạng thái chơi game
                    currentState_ = UIState::PLAYING;
                    validateCurrentMove();
                    return;
                }
            }
        }
    }
}

bool GameUI::isTileTemporarilyPlacedAt(int r, int c) const {
    for (const auto& placedTile : currentMoveTiles_) {
        if (placedTile.boardRow == r && placedTile.boardCol == c) {
            return true;
        }
    }
    return false;
}