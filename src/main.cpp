#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

// Include các file game của bạn
#include "core/Game.h"
#include "core/Board.h"
#include "core/Player.h"

// --- Hằng số cho cửa sổ và bàn cờ ---
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BOARD_SIZE_PX = 450; // Kích thước bàn cờ trên màn hình
const int CELL_SIZE = BOARD_SIZE_PX / Board::SIZE; // Kích thước 1 ô
const int BOARD_X_OFFSET = 50;
const int BOARD_Y_OFFSET = 50;


// --- Hàm trợ giúp để vẽ chữ lên màn hình ---
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}


// --- Hàm trợ giúp để vẽ bàn cờ ---
void renderBoard(SDL_Renderer* renderer, TTF_Font* font, const Board& board) {
    SDL_Rect cellRect = { 0, 0, CELL_SIZE, CELL_SIZE };
    SDL_Color textColor = { 0, 0, 0, 255 }; // Màu đen

    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            cellRect.x = BOARD_X_OFFSET + c * CELL_SIZE;
            cellRect.y = BOARD_Y_OFFSET + r * CELL_SIZE;

            // Tô màu cho các ô đặc biệt (chỉ làm ví dụ)
            const auto& cell = board.getCell(r, c);
            switch (cell.type) {
                case Board::CellType::CENTER:
                case Board::CellType::DOUBLE_WORD: SDL_SetRenderDrawColor(renderer, 255, 192, 203, 255); break; // Màu hồng
                case Board::CellType::TRIPLE_WORD: SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); break;     // Màu đỏ
                case Board::CellType::DOUBLE_LETTER: SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255); break; // Xanh nhạt
                case Board::CellType::TRIPLE_LETTER: SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); break;     // Xanh đậm
                default: SDL_SetRenderDrawColor(renderer, 210, 180, 140, 255); break; // Màu be
            }
            SDL_RenderFillRect(renderer, &cellRect);

            // Vẽ đường viền
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen
            SDL_RenderDrawRect(renderer, &cellRect);

            // Vẽ chữ cái nếu có
            if (cell.hasTile()) {
                std::string letterStr(1, cell.tile.getLetter());
                renderText(renderer, font, letterStr, cellRect.x + 10, cellRect.y + 5, textColor);
            }
        }
    }
}


// --- Hàm trợ giúp để vẽ khay chữ của người chơi ---
void renderRack(SDL_Renderer* renderer, TTF_Font* font, const Player* player) {
    if (!player) return;
    
    SDL_Rect tileRect = { 0, 0, 40, 40 };
    SDL_Color tileColor = { 255, 255, 224, 255 }; // Vàng nhạt
    SDL_Color textColor = { 0, 0, 0, 255 }; // Đen

    int startX = 50;
    int startY = BOARD_Y_OFFSET + BOARD_SIZE_PX + 20;

    const auto& rack = player->getRack();
    for (size_t i = 0; i < rack.size(); ++i) {
        tileRect.x = startX + i * 45;
        tileRect.y = startY;

        SDL_SetRenderDrawColor(renderer, tileColor.r, tileColor.g, tileColor.b, 255);
        SDL_RenderFillRect(renderer, &tileRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &tileRect);

        std::string letterStr(1, rack[i].getLetter());
        renderText(renderer, font, letterStr, tileRect.x + 15, tileRect.y + 10, textColor);
    }
}


int main(int argc, char* argv[]) {
    // --- KHỞI TẠO SDL VÀ CÁC THÀNH PHẦN ---
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Scrabble Game Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("arial.ttf", 18); // Đảm bảo có file arial.ttf

    if (!window || !renderer || !font) {
        std::cerr << "Failed to create window, renderer, or load font." << std::endl;
        return 1;
    }

    // --- KHỞI TẠO GAME LOGIC ---
    Game game;
    game.startNewGame(1); // Bắt đầu game với 1 người chơi và 1 AI

    bool running = true;
    SDL_Event e;

    bool humanPlayed = false; // Cờ để đảm bảo logic chỉ chạy 1 lần

    // --- VÒNG LẶP CHÍNH CỦA GAME ---
    while (running) {
        // Xử lý sự kiện (như bấm nút đóng cửa sổ)
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        // --- LOGIC TEST (Chỉ chạy một lần) ---
        // Phần này mô phỏng việc người chơi và AI thực hiện nước đi.
        if (!humanPlayed) {
            std::cout << "--- Lượt của người chơi Human ---" << std::endl;
            // Người chơi thử đặt từ "HELLO" tại (7, 5)
            // Thay đổi từ này để test các trường hợp khác nhau
            // Lưu ý: Nước đi này có thể thất bại nếu người chơi không có đủ chữ cái.
            game.playWord(0, "HELLO", 7, 5, true); 

            // Sau khi người chơi đi, lượt sẽ tự động chuyển cho AI (do nextTurn() được gọi bên trong playWord)
            humanPlayed = true;
        }


        // --- VẼ LÊN MÀN HÌNH ---
        // Xóa màn hình với màu xanh nhạt
        SDL_SetRenderDrawColor(renderer, 200, 220, 255, 255);
        SDL_RenderClear(renderer);

        // Lấy thông tin người chơi hiện tại
        const Player* currentPlayer = game.getPlayer(game.getCurrentPlayerId());
        
        // Vẽ các thành phần
        renderBoard(renderer, font, game.getBoard());
        if(currentPlayer) {
            renderRack(renderer, font, currentPlayer);

            // Hiển thị thông tin
            SDL_Color textColor = {0, 0, 0, 255};
            std::string info = currentPlayer->getName() + " | Score: " + std::to_string(currentPlayer->getScore());
            renderText(renderer, font, info, 50, 15, textColor);
        }
        
        // Cập nhật màn hình
        SDL_RenderPresent(renderer);
    }

    // --- DỌN DẸP VÀ KẾT THÚC ---
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}