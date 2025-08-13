#include "UI/GameUI.h"
#include <iostream>
#include <string>
#include <algorithm>

const SDL_Color COLOR_BACKGROUND = { 34, 40, 49, 255 };    // Xám xanh đậm
const SDL_Color COLOR_BOARD_BG = { 20, 25, 30, 255 };      // Nền bàn cờ
const SDL_Color COLOR_NORMAL_CELL = { 205, 193, 180, 255 }; // Be
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
    fontSmall_ = TTF_OpenFont("assets/font/Pixel.ttf", 11);
    fontBig_ = TTF_OpenFont("assets/font/Pixel.ttf", 36);
    fontTitle_ = TTF_OpenFont("assets/font/Pixel.ttf", 96);

    if (!window_ || !renderer_ || !fontNormal_ || !fontSmall_ || !fontBig_) return false;

    defineLayout();
    return true;
}

void GameUI::defineLayout() {
    boardRect_ = { BOARD_X, BOARD_Y, BOARD_SIZE_PX, BOARD_SIZE_PX };
    rackRect_ = { BOARD_X, boardRect_.y + boardRect_.h + 15, 520, 60 };
    buttonsRect_ = { rackRect_.x, rackRect_.y + rackRect_.h + 10, 130, 45 };
    skipButtonRect_ = { buttonsRect_.x + buttonsRect_.w + 15, buttonsRect_.y, 80, 45 };
    resetButtonRect_ = { skipButtonRect_.x + skipButtonRect_.w + 15, skipButtonRect_.y, 90, 45 };
    sidebarRect_ = { BOARD_AREA_WIDTH, 0, SCREEN_WIDTH - BOARD_AREA_WIDTH, SCREEN_HEIGHT };
    
    int sidebarPadding = 30;
    int panelSpacing = 20;
    
    playerInfoRect_ = { sidebarRect_.x + sidebarPadding, sidebarRect_.y + 40, sidebarRect_.w - 2 * sidebarPadding, 80 };
    opponentInfoRect_ = { playerInfoRect_.x, playerInfoRect_.y + playerInfoRect_.h + panelSpacing, playerInfoRect_.w, 80 };
    tileBagRect_ = { playerInfoRect_.x, opponentInfoRect_.y + opponentInfoRect_.h + panelSpacing, playerInfoRect_.w, 200 };
    turnHistoryRect_ = { playerInfoRect_.x, tileBagRect_.y + tileBagRect_.h + panelSpacing, playerInfoRect_.w, 250 };
    playButtonRect_ = { SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 + 50, 150, 60 };
    swapButtonRect_ = { resetButtonRect_.x + resetButtonRect_.w + 15, resetButtonRect_.y, 90, 45 };
    confirmSwapButtonRect_ = buttonsRect_; 

    dynamicSwapButtonRect_ = swapButtonRect_;
    dynamicConfirmSwapButtonRect_ = confirmSwapButtonRect_;
    dynamicSubmitButtonRect_ = buttonsRect_;
    dynamicSkipButtonRect_ = skipButtonRect_;
    dynamicResetButtonRect_ = resetButtonRect_;
    dynamicPlayButtonRect_ = playButtonRect_;
}

void GameUI::close() {
    TTF_CloseFont(fontNormal_);
    TTF_CloseFont(fontSmall_);
    TTF_CloseFont(fontBig_);
    TTF_CloseFont(fontTitle_);
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    TTF_Quit();
    SDL_Quit();
}

void GameUI::run() {
    running_ = true;
    while (running_) {
        handleEvents();
        update();
        updateGame();  
        render();      
        SDL_Delay(16);
    }
}

void GameUI::update() {
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
        
        // Thêm logic hover cho nút CONFIRM SWAP khi ở trạng thái chọn
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
            SDL_Delay(500);
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

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Player* player = game_.getPlayer(0);
        if (!player) continue;

        // --- XỬ LÝ NHẤN CHUỘT ---
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (isDragging_) continue; // Nếu đang kéo rồi thì bỏ qua

            // Nút RESET
            if (mouseX >= resetButtonRect_.x && mouseX < resetButtonRect_.x + resetButtonRect_.w &&
                mouseY >= resetButtonRect_.y && mouseY < resetButtonRect_.y + resetButtonRect_.h) {
                currentMoveTiles_.clear();
                continue; 
            }
            // Nút SKIP
            if (mouseX >= skipButtonRect_.x && mouseX < skipButtonRect_.x + skipButtonRect_.w &&
                mouseY >= skipButtonRect_.y && mouseY < skipButtonRect_.y + skipButtonRect_.h) {
                currentMoveTiles_.clear();
                game_.passTurn(0); 
                continue;
            }
            // Nút SUBMIT
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

            if (mouseX >= swapButtonRect_.x && mouseX < swapButtonRect_.x + swapButtonRect_.w &&
                mouseY >= swapButtonRect_.y && mouseY < swapButtonRect_.y + swapButtonRect_.h) {                
                currentMoveTiles_.clear();
                tilesToSwapIndices_.clear();
                currentState_ = UIState::SELECTING_SWAP;
                continue;
            } 

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
                    goto end_mouse_down_check;
                }
            }

            for (size_t i = 0; i < player->getRack().size(); ++i) {
                bool alreadyPlaced = false;
                for(const auto& placedTile : currentMoveTiles_) {
                    if (placedTile.originalRackIndex == (int)i) { alreadyPlaced = true; break; }
                }
                if (alreadyPlaced) continue;
                SDL_Rect tileRect = { rackRect_.x + (int)i * (TILE_SIZE + 5), rackRect_.y, TILE_SIZE, TILE_SIZE };
                if (mouseX >= tileRect.x && mouseX < tileRect.x + tileRect.w && mouseY >= tileRect.y && mouseY < tileRect.y + tileRect.h) {
                    isDragging_ = true;
                    draggedRackIndex_ = i;
                    draggedTile_ = player->getRack()[i];
                    draggedBoardTileIndex_ = -1;
                    dragOffset_ = { mouseX - tileRect.x, mouseY - tileRect.y };
                    break;
                }
            }
            end_mouse_down_check:;
        } 
        else if (e.type == SDL_MOUSEBUTTONUP) {
            if (isDragging_) {
                bool dropped_successfully = false;
                if (mouseX >= boardRect_.x && mouseX < boardRect_.x + boardRect_.w && mouseY >= boardRect_.y && mouseY < boardRect_.y + boardRect_.h) {
                    int col = (mouseX - boardRect_.x) / CELL_SIZE;
                    int row = (mouseY - boardRect_.y) / CELL_SIZE;
                    bool isOccupied = game_.getBoard().hasTile(row, col);
                    for(const auto& placedTile : currentMoveTiles_) {
                        if (placedTile.boardRow == row && placedTile.boardCol == col) { isOccupied = true; break; }
                    }
                    if (!isOccupied) {
                        currentMoveTiles_.push_back({draggedTile_, row, col, draggedRackIndex_});
                        dropped_successfully = true;
                    }
                }
                if (!dropped_successfully && mouseX >= rackRect_.x && mouseX < rackRect_.x + rackRect_.w && mouseY >= rackRect_.y && mouseY < rackRect_.y + rackRect_.h) {
                    dropped_successfully = true;
                }
                if (!dropped_successfully) {
                    if (draggedBoardTileIndex_ != -1) {
                         currentMoveTiles_.insert(currentMoveTiles_.begin() + draggedBoardTileIndex_, {draggedTile_, originalDragPos_.y, originalDragPos_.x, draggedRackIndex_});
                    }
                }
                isDragging_ = false;
                draggedRackIndex_ = -1;
                draggedBoardTileIndex_ = -1;
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

    // Vẽ tile đang được kéo (luôn vẽ sau cùng để nó nổi lên trên)
    if (isDragging_) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY); // Lấy vị trí chuột mới nhất
        renderTile(draggedTile_, mouseX - dragOffset_.x, mouseY - dragOffset_.y);
    }

    if (invalidMoveTimestamp_ != 0) {
        Uint32 currentTime = SDL_GetTicks();
        // Nếu chưa đủ 2 giây (2000 mili giây)
        if (currentTime - invalidMoveTimestamp_ < 2000) {
            // Vẽ chữ "INVALID" màu đỏ giữa bàn cờ
            SDL_Color red = {255, 0, 0, 255};
            renderText("INVALID MOVE", boardRect_.x, boardRect_.y, boardRect_.w, boardRect_.h, fontBig_, red);
        } else {
            // Nếu đã quá 2 giây, reset bộ đếm giờ
            invalidMoveTimestamp_ = 0;
        }
    }

    SDL_RenderPresent(renderer_);
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

            // 1. Xử lý click nút CONFIRM SWAP
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
                // Quay lại trạng thái chơi game
                tilesToSwapIndices_.clear();
                currentState_ = UIState::PLAYING;
                continue;
            }

            // 2. Xử lý click nút CANCEL (dùng nút RESET)
            if (mouseX >= resetButtonRect_.x && mouseX < resetButtonRect_.x + resetButtonRect_.w &&
                mouseY >= resetButtonRect_.y && mouseY < resetButtonRect_.y + resetButtonRect_.h) {
                
                tilesToSwapIndices_.clear();
                currentState_ = UIState::PLAYING;
                continue;
            }

            // 3. Xử lý click chọn/bỏ chọn tile trên khay
            const auto& rack = player->getRack();
            for (size_t i = 0; i < rack.size(); ++i) {
                SDL_Rect tileRect = { rackRect_.x + (int)i * (TILE_SIZE + 5), rackRect_.y, TILE_SIZE, TILE_SIZE };
                if (mouseX >= tileRect.x && mouseX < tileRect.x + tileRect.w &&
                    mouseY >= tileRect.y && mouseY < tileRect.y + tileRect.h) {
                    
                    auto it = std::find(tilesToSwapIndices_.begin(), tilesToSwapIndices_.end(), i);
                    if (it != tilesToSwapIndices_.end()) {
                        // Nếu đã chọn, bỏ chọn
                        tilesToSwapIndices_.erase(it);
                    } else {
                        // Nếu chưa chọn, thêm vào danh sách
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
            if (mouseX >= playButtonRect_.x && mouseX < playButtonRect_.x + playButtonRect_.w &&
                mouseY >= playButtonRect_.y && mouseY < playButtonRect_.y + playButtonRect_.h) {
                
                // *** BẮT ĐẦU GAME KHI CLICK VÀO NÚT PLAY ***
                game_.startNewGame(1);
                currentState_ = UIState::PLAYING;
            }
        }
    }
}

void GameUI::renderMenu() {
    // 1. Định nghĩa các màu sắc của cầu vồng
    const std::vector<SDL_Color> rainbowColors = {
        {255, 0, 0, 255},    // Đỏ
        {255, 165, 0, 255},  // Cam
        {255, 255, 0, 255},  // Vàng
        {0, 255, 0, 255},    // Lục
        {0, 0, 255, 255},    // Lam
        {75, 0, 130, 255},   // Chàm
        {238, 130, 238, 255} // Tím
    };

    // 2. Logic vẽ tiêu đề "SCRABBLE" từng chữ một
    std::string title = "SCRABBLE";
    int totalWidth = 0;
    int charHeight = 0;

    // Tính toán tổng chiều rộng của cả chuỗi để căn giữa
    for (char c : title) {
        int w;
        TTF_SizeText(fontTitle_, std::string(1, c).c_str(), &w, &charHeight);
        totalWidth += w;
    }

    // Vị trí bắt đầu vẽ (căn giữa theo chiều ngang)
    int currentX = (SCREEN_WIDTH - totalWidth) / 2;
    int y = SCREEN_HEIGHT / 2 - 150;

    // Lặp qua từng ký tự để vẽ
    for (size_t i = 0; i < title.length(); ++i) {
        std::string letter(1, title[i]);
        // Chọn màu tương ứng, lặp lại nếu hết màu
        SDL_Color color = rainbowColors[i % rainbowColors.size()]; 
        
        // Vẽ ký tự
        renderText(letter, currentX, y, fontTitle_, color);

        // Cập nhật vị trí X cho ký tự tiếp theo
        int w;
        TTF_SizeText(fontTitle_, letter.c_str(), &w, nullptr);
        currentX += w;
    }

    // 3. Vẽ nút "PLAY" (giữ nguyên)
    SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
    SDL_RenderFillRect(renderer_, &dynamicPlayButtonRect_); 
    renderText("PLAY", dynamicPlayButtonRect_.x, dynamicPlayButtonRect_.y, dynamicPlayButtonRect_.w, dynamicPlayButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
}

void GameUI::handleEvents() {
    if (currentState_ == UIState::MAIN_MENU) {
        handleMenuEvents();
    } else if (currentState_ == UIState::SELECTING_SWAP) {
        handleSwapSelectionEvents();
    } else { // PLAYING hoặc GAME_OVER
        handleGameEvents();
    }
}

void GameUI::render() {
    SDL_SetRenderDrawColor(renderer_, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, 255);
    SDL_RenderClear(renderer_);

    if (currentState_ == UIState::MAIN_MENU) {
        renderMenu();
    } else { // PLAYING hoặc GAME_OVER
        renderGame();
    }

    SDL_RenderPresent(renderer_);
}

void GameUI::renderBoard() {
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

            // *** LOGIC ĐÃ ĐƯỢC SẮP XẾP LẠI HOÀN TOÀN ***

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
                    renderText(label, cellRect.x, cellRect.y, cellRect.w, cellRect.h, fontNormal_, COLOR_TEXT_DARK);
                }
            }
        }
    }

    // 3. Cuối cùng, vẽ các ô chữ người chơi đang đặt tạm
    for (const auto& placedTile : currentMoveTiles_) {
        int x = boardRect_.x + placedTile.boardCol * CELL_SIZE;
        int y = boardRect_.y + placedTile.boardRow * CELL_SIZE;
        renderTile(placedTile.tile, x, y);
    }
}

void GameUI::renderRack() {
    Player* player = game_.getPlayer(game_.getCurrentPlayerId());
    if (!player) return;

    const auto& rack = player->getRack();
    for (size_t i = 0; i < rack.size(); ++i) {
        // Ẩn tile nếu nó đang được kéo
        if (isDragging_ && (int)i == draggedRackIndex_) {
            continue;
        }

        // Ẩn tile nếu nó đã được đặt tạm lên bàn cờ
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

        // Vẽ tile bình thường trên khay
        int x = rackRect_.x + i * (TILE_SIZE + 5);
        int y = rackRect_.y;
        // *** THÊM LOGIC HIGHLIGHT KHI CHỌN SWAP ***
        bool isSelectedForSwap = false;
        if (currentState_ == UIState::SELECTING_SWAP) {
            auto it = std::find(tilesToSwapIndices_.begin(), tilesToSwapIndices_.end(), i);
            if (it != tilesToSwapIndices_.end()) {
                isSelectedForSwap = true;
            }
        }

        // Nếu được chọn, vẽ một lớp mờ màu xanh lên trên
        if (isSelectedForSwap) {
            renderTile(rack[i], x, y);
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer_, 0, 173, 181, 100); // Màu xanh mờ
            SDL_Rect highlightRect = {x, y, TILE_SIZE, TILE_SIZE};
            SDL_RenderFillRect(renderer_, &highlightRect);
            SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);
        } else {
            renderTile(rack[i], x, y);
        }
    }
}

void GameUI::renderTile(const Tile& tile, int x, int y) {
    SDL_Rect tileRect = { x + 1, y + 1, TILE_SIZE, TILE_SIZE };
    SDL_SetRenderDrawColor(renderer_, COLOR_TILE.r, COLOR_TILE.g, COLOR_TILE.b, 255);
    SDL_RenderFillRect(renderer_, &tileRect);

    SDL_SetRenderDrawColor(renderer_, 80, 50, 20, 255);
    SDL_RenderDrawRect(renderer_, &tileRect);

    if (!tile.isBlank()) {
        std::string letterStr(1, tile.getLetter());
        renderText(letterStr, tileRect.x, tileRect.y, tileRect.w, tileRect.h - 5, fontTile_, COLOR_TEXT_DARK);
        
        std::string valueStr = std::to_string(tile.getValue());
        renderText(valueStr, tileRect.x + tileRect.w - 12, tileRect.y + tileRect.h - 16, fontSmall_, COLOR_TEXT_DARK);
    }
}

void GameUI::renderTileBagPanel(const SDL_Rect& rect) {
    SDL_SetRenderDrawColor(renderer_, 40, 50, 60, 255);
    SDL_RenderFillRect(renderer_, &rect);
    renderText("Tile Bag", rect.x, rect.y + 10, rect.w, 20, fontNormal_, COLOR_TEXT_LIGHT);
    std::string remaining = "Remaining: " + std::to_string(game_.getTileBag().remainingTiles());
    renderText(remaining, rect.x, rect.y + 35, rect.w, 20, fontNormal_, COLOR_TEXT_LIGHT);
    // (Ở đây bạn có thể thêm logic để vẽ chi tiết các chữ cái còn lại)
}

void GameUI::renderHistoryPanel(const SDL_Rect& rect) {
    SDL_SetRenderDrawColor(renderer_, 40, 50, 60, 255);
    SDL_RenderFillRect(renderer_, &rect);
    // Thay đổi tiêu đề cho phù hợp
    renderText("Last Move", rect.x, rect.y + 10, rect.w, 20, fontNormal_, COLOR_TEXT_LIGHT);

    const auto& history = game_.getTurnHistory();

    // 1. Kiểm tra xem đã có lượt đi nào trong lịch sử chưa
    if (history.empty()) {
        return; // Nếu chưa có, không vẽ gì cả
    }

    // 2. Lấy ra chỉ lượt đi cuối cùng
    const auto& lastTurn = history.back();

    // 3. Tạo chuỗi văn bản để hiển thị (logic giữ nguyên)
    std::string text;
    if (lastTurn.isPass) {
        text = lastTurn.playerName + " passed.";
    } else {
        text = lastTurn.playerName + " played move '" + lastTurn.word + "' for " + std::to_string(lastTurn.score) + " pts.";
    }

    // 4. Vẽ văn bản của lượt đi cuối cùng đó
    int y_offset = 40;
    renderText(text, rect.x + 10, rect.y + y_offset, fontSmall_, COLOR_TEXT_LIGHT);
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

void GameUI::renderSidebar() {
    // Vẽ nền chính của sidebar
    SDL_SetRenderDrawColor(renderer_, COLOR_SIDEBAR.r, COLOR_SIDEBAR.g, COLOR_SIDEBAR.b, 255);
    SDL_RenderFillRect(renderer_, &sidebarRect_);

    Player* human_player = game_.getPlayer(0);
    Player* ai_player = game_.getPlayer(1);

    renderPlayerPanel(human_player, 0, playerInfoRect_, false); 
    renderPlayerPanel(ai_player, 1, opponentInfoRect_, true);
    renderTileBagPanel(tileBagRect_);
    renderHistoryPanel(turnHistoryRect_);
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