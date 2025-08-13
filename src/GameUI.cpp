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

const int SCREEN_WIDTH = 930;
const int SCREEN_HEIGHT = 750;
const int BOARD_X = 30;
const int BOARD_Y = 30;
const int BOARD_SIZE_PX = 600;
const int CELL_SIZE = BOARD_SIZE_PX / Board::SIZE; // 40px
const int TILE_SIZE = 38;

GameUI::GameUI(Game& gameLogic) : game_(gameLogic) { if (!init()) { /* Lỗi */ } }
GameUI::~GameUI() { close(); }

bool GameUI::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) return false;
    window_ = SDL_CreateWindow("Scrabble", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    fontNormal_ = TTF_OpenFont("assets/font/Pixel.ttf", 16);
    fontSmall_ = TTF_OpenFont("assets/font/Pixel.ttf", 11);
    fontBig_ = TTF_OpenFont("assets/font/Pixel.ttf", 26);

    if (!window_ || !renderer_ || !fontNormal_ || !fontSmall_ || !fontBig_) return false;

    defineLayout();
    return true;
}

void GameUI::defineLayout() {
    boardRect_ = { BOARD_X, BOARD_Y, BOARD_SIZE_PX, BOARD_SIZE_PX };
    rackRect_ = { BOARD_X, boardRect_.y + boardRect_.h + 20, 450, 60 };
    sidebarRect_ = { boardRect_.x + boardRect_.w + 30, BOARD_Y, 240, 680 };
    buttonsRect_ = { rackRect_.x + rackRect_.w + 20, rackRect_.y, 130, 45 };
    playButtonRect_ = { SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 + 50, 150, 60 };
    dynamicSubmitButtonRect_ = buttonsRect_;
    dynamicPlayButtonRect_ = playButtonRect_;
    skipButtonRect_ = { buttonsRect_.x + buttonsRect_.w + 10, buttonsRect_.y, 80, 45 };
    dynamicSubmitButtonRect_ = buttonsRect_;
    dynamicPlayButtonRect_ = playButtonRect_;
    dynamicSkipButtonRect_ = skipButtonRect_;
}

void GameUI::close() {
    TTF_CloseFont(fontNormal_);
    TTF_CloseFont(fontSmall_);
    TTF_CloseFont(fontBig_);
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

    if (currentState_ == UIState::PLAYING) {
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
        // Luôn kiểm tra sự kiện thoát game
        if (e.type == SDL_QUIT) {
            running_ = false;
        }

        // Chỉ xử lý các tương tác nếu game đang chạy và đến lượt của người chơi (ID = 0)
        if (game_.getState() != Game::State::PLAYING || game_.getCurrentPlayerId() != 0) {
            continue;
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Player* player = game_.getPlayer(0);
        if (!player) continue;
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (mouseX >= skipButtonRect_.x && mouseX < skipButtonRect_.x + skipButtonRect_.w &&
                mouseY >= skipButtonRect_.y && mouseY < skipButtonRect_.y + skipButtonRect_.h) {
                currentMoveTiles_.clear();
                game_.passTurn(0); 
                continue;
            }

            if (mouseX >= buttonsRect_.x && mouseX < buttonsRect_.x + buttonsRect_.w && 
                mouseY >= buttonsRect_.y && mouseY < buttonsRect_.y + buttonsRect_.h) {
                
                if (!currentMoveTiles_.empty()) {
                    bool allSameRow = true, allSameCol = true;
                    int firstRow = currentMoveTiles_[0].boardRow;
                    int firstCol = currentMoveTiles_[0].boardCol;
                    for (const auto& t : currentMoveTiles_) {
                        if (t.boardRow != firstRow) allSameRow = false;
                        if (t.boardCol != firstCol) allSameCol = false;
                    }
                    if (!allSameRow && !allSameCol) {
                        std::cout << "Placed tiles not aligned!" << std::endl;
                        currentMoveTiles_.clear();  // Clear invalid
                        continue;
                    }
                    // Sắp xếp các ô chữ đã đặt để xác định hướng đi
                    std::sort(currentMoveTiles_.begin(), currentMoveTiles_.end(), [](const auto& a, const auto& b) {
                        if (a.boardRow != b.boardRow) return a.boardRow < b.boardRow;
                        return a.boardCol < b.boardCol;
                    });

                    int startRow = currentMoveTiles_.front().boardRow;
                    int startCol = currentMoveTiles_.front().boardCol;
                    bool isHorizontal = (currentMoveTiles_.size() > 1) ? (currentMoveTiles_[0].boardRow == currentMoveTiles_[1].boardRow) : true;

                    // Xử lý trường hợp chỉ đặt một ô chữ
                    if (currentMoveTiles_.size() == 1) {
                         bool hNeighbor = game_.getBoard().hasTile(startRow, startCol - 1) || game_.getBoard().hasTile(startRow, startCol + 1);
                         if (!hNeighbor && (game_.getBoard().hasTile(startRow - 1, startCol) || game_.getBoard().hasTile(startRow + 1, startCol))) {
                            isHorizontal = false;
                         }
                    }

                    std::string fullWord = "";
                    int r = startRow, c = startCol;
                    if (isHorizontal) {
                        while (c > 0 && (game_.getBoard().hasTile(r, c - 1) || std::any_of(currentMoveTiles_.begin(), currentMoveTiles_.end(), [&](const auto& t){return t.boardRow==r && t.boardCol==c-1;}))) c--;
                    } else {
                        while (r > 0 && (game_.getBoard().hasTile(r - 1, c) || std::any_of(currentMoveTiles_.begin(), currentMoveTiles_.end(), [&](const auto& t){return t.boardRow==r-1 && t.boardCol==c;}))) r--;
                    }
                    int finalStartRow = r, finalStartCol = c;

                    while (r < Board::SIZE && c < Board::SIZE) {
                        bool isTempTile = false;
                        for (const auto& pTile : currentMoveTiles_) {
                            if (pTile.boardRow == r && pTile.boardCol == c) {
                                fullWord += pTile.tile.getLetter(); isTempTile = true; break;
                            }
                        }
                        if (!isTempTile) {
                            if (game_.getBoard().hasTile(r, c)) {
                                fullWord += game_.getBoard().getCell(r, c).tile.getLetter();
                            } else break;
                        }
                        if (isHorizontal) c++; else r++;
                    }

                    // Tạo chuỗi chữ chỉ từ các ô trên khay
                    std::string wordFromRack;
                    for(const auto& pTile : currentMoveTiles_) wordFromRack += pTile.tile.getLetter();
                    
                    // Gửi nước đi đến logic game
                    bool success = game_.playWord(0, wordFromRack, fullWord, finalStartRow, finalStartCol, isHorizontal);
                    if (success) {
                        currentMoveTiles_.clear();
                    } else {
                        // Nếu nước đi không hợp lệ, xóa các ô chữ tạm để chúng quay về khay
                        currentMoveTiles_.clear();
                        invalidMoveTimestamp_ = SDL_GetTicks();
                    }
                }
                continue; // Dừng xử lý sự kiện này sau khi click nút
            }

            // 2. Kiểm tra click vào một ô chữ trên khay để bắt đầu kéo
            for (size_t i = 0; i < player->getRack().size(); ++i) {
                bool alreadyPlaced = false;
                for(const auto& placedTile : currentMoveTiles_) {
                    if (placedTile.originalRackIndex == (int)i) {
                        alreadyPlaced = true;
                        break;
                    }
                }
                if (alreadyPlaced) continue;

                SDL_Rect tileRect = { rackRect_.x + (int)i * (TILE_SIZE + 5), rackRect_.y, TILE_SIZE, TILE_SIZE };
                if (mouseX >= tileRect.x && mouseX < tileRect.x + tileRect.w && mouseY >= tileRect.y && mouseY < tileRect.y + tileRect.h) {
                    isDragging_ = true;
                    draggedRackIndex_ = i;
                    draggedTile_ = player->getRack()[i];
                    dragOffset_ = { mouseX - tileRect.x, mouseY - tileRect.y };
                    break;
                }
            }
        } 
        // Xử lý sự kiện thả chuột
        else if (e.type == SDL_MOUSEBUTTONUP) {
            if (isDragging_) {
                 if (mouseX >= boardRect_.x && mouseX < boardRect_.x + boardRect_.w && 
                    mouseY >= boardRect_.y && mouseY < boardRect_.y + boardRect_.h) {
                    
                    int col = (mouseX - boardRect_.x) / CELL_SIZE;
                    int row = (mouseY - boardRect_.y) / CELL_SIZE;

                    bool isOccupied = game_.getBoard().hasTile(row, col);
                    for(const auto& placedTile : currentMoveTiles_) {
                        if (placedTile.boardRow == row && placedTile.boardCol == col) {
                            isOccupied = true;
                            break;
                        }
                    }
                    if (!isOccupied) {
                        currentMoveTiles_.push_back({draggedTile_, row, col, draggedRackIndex_});
                    }
                }
                // Kết thúc trạng thái kéo
                isDragging_ = false;
                draggedRackIndex_ = -1;
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
        SDL_GetMouseState(&mouseX, &mouseY);
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
    renderText("SCRABBLE", 0, SCREEN_HEIGHT / 2 - 150, SCREEN_WIDTH, 100, fontBig_, COLOR_TEXT_LIGHT);
    SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
    SDL_RenderFillRect(renderer_, &dynamicPlayButtonRect_); 
    renderText("PLAY", dynamicPlayButtonRect_.x, dynamicPlayButtonRect_.y, dynamicPlayButtonRect_.w, dynamicPlayButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
}

void GameUI::handleEvents() {
    if (currentState_ == UIState::MAIN_MENU) {
        handleMenuEvents();
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
        renderTile(rack[i], x, y);
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
        renderText(letterStr, tileRect.x, tileRect.y, tileRect.w, tileRect.h - 5, fontBig_, COLOR_TEXT_DARK);
        
        std::string valueStr = std::to_string(tile.getValue());
        renderText(valueStr, tileRect.x + tileRect.w - 12, tileRect.y + tileRect.h - 14, fontSmall_, COLOR_TEXT_DARK);
    }
}

void GameUI::renderSidebar() {
    // Vẽ nền cho sidebar
    SDL_SetRenderDrawColor(renderer_, COLOR_SIDEBAR.r, COLOR_SIDEBAR.g, COLOR_SIDEBAR.b, 255);
    SDL_RenderFillRect(renderer_, &sidebarRect_);
    
    // Chuẩn bị các màu chữ
    SDL_Color white = {238, 238, 238, 255};
    SDL_Color yellow_highlight = {255, 215, 0, 255}; // Màu vàng để highlight
    
    // Lấy thông tin 2 người chơi
    Player* human_player = game_.getPlayer(0);
    Player* ai_player = game_.getPlayer(1);
    
    // Vẽ thông tin người chơi Human
    if (human_player) {
        // Chọn màu chữ: nếu là lượt của Human (ID=0) thì dùng màu vàng, ngược lại dùng màu trắng
        SDL_Color p1_color = (game_.getCurrentPlayerId() == 0) ? yellow_highlight : white;
        std::string p1_text = human_player->getName() + ": " + std::to_string(human_player->getScore()) + " pts";
        renderText(p1_text, sidebarRect_.x + 10, sidebarRect_.y + 20, fontNormal_, p1_color);
    }

    // Vẽ thông tin người chơi AI
    if (ai_player) {
        // Chọn màu chữ: nếu là lượt của AI (ID=1) thì dùng màu vàng, ngược lại dùng màu trắng
        SDL_Color p2_color = (game_.getCurrentPlayerId() == 1) ? yellow_highlight : white;
        std::string p2_text = ai_player->getName() + ": " + std::to_string(ai_player->getScore()) + " pts";
        renderText(p2_text, sidebarRect_.x + 10, sidebarRect_.y + 50, fontNormal_, p2_color);
    }
    
    // Vẽ thông tin số chữ còn lại trong túi
    renderText("Tiles in Bag: " + std::to_string(game_.getTileBag().remainingTiles()), sidebarRect_.x + 10, sidebarRect_.y + 90, fontNormal_, white);
}

void GameUI::renderButtons() {
    SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
    SDL_RenderFillRect(renderer_, &dynamicSubmitButtonRect_); 
    renderText("SUBMIT", dynamicSubmitButtonRect_.x, dynamicSubmitButtonRect_.y, dynamicSubmitButtonRect_.w, dynamicSubmitButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
    SDL_SetRenderDrawColor(renderer_, COLOR_BUTTON.r, COLOR_BUTTON.g, COLOR_BUTTON.b, 255);
    SDL_RenderFillRect(renderer_, &dynamicSkipButtonRect_);
    renderText("SKIP", dynamicSkipButtonRect_.x, dynamicSkipButtonRect_.y, dynamicSkipButtonRect_.w, dynamicSkipButtonRect_.h, fontNormal_, COLOR_TEXT_LIGHT);
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