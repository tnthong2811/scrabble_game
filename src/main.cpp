#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include "../include/core/dictionary/trie_dictionary.hpp"
#include "../include/core/dictionary/dictionary_factory.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int INPUT_BOX_WIDTH = 400;
const int INPUT_BOX_HEIGHT = 60;
const SDL_Color BG_COLOR = { 240, 240, 240, 255 };
const SDL_Color BOX_COLOR = { 255, 255, 255, 255 };
const SDL_Color TEXT_COLOR = { 0, 0, 0, 255 };
const SDL_Color OK_COLOR = { 0, 255, 0, 255 };
const SDL_Color NOT_FOUND_COLOR = { 255, 0, 0, 255 };

int main(int argc, char* args[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Dictionary Checker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Tải font
    TTF_Font* font = TTF_OpenFont("assets/font/Pixel.ttf", 28);
    if (font == nullptr) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo từ điển
    auto dictionary = DictionaryFactory::create(DictionaryType::TRIE);
    if (!dictionary->load("assets/dictionary/dictionary.txt")) {
        std::cerr << "Failed to load dictionary!" << std::endl;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Bắt đầu nhận sự kiện nhập liệu văn bản
    SDL_StartTextInput(); // <--- Đã thêm dòng này

    // Biến nhập liệu và trạng thái
    std::string inputText = "";
    std::string statusText = "";
    SDL_Rect inputBox = { (SCREEN_WIDTH - INPUT_BOX_WIDTH) / 2, (SCREEN_HEIGHT - INPUT_BOX_HEIGHT) / 2, INPUT_BOX_WIDTH, INPUT_BOX_HEIGHT };
    SDL_Rect statusBox = { inputBox.x, inputBox.y + INPUT_BOX_HEIGHT + 10, INPUT_BOX_WIDTH, INPUT_BOX_HEIGHT };
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty()) {
                    inputText.pop_back();
                    statusText.clear();
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    if (dictionary->contains(inputText)) {
                        statusText = "OK";
                    } else {
                        statusText = "Not Found";
                    }
                    inputText.clear(); // Xóa input sau khi kiểm tra
                } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
            } else if (e.type == SDL_TEXTINPUT) {
                inputText += e.text.text;
                statusText.clear(); // Xóa trạng thái khi nhập mới
            }
        }

        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
        SDL_RenderClear(renderer);

        // Vẽ ô nhập liệu
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &inputBox);
        SDL_SetRenderDrawColor(renderer, BOX_COLOR.r, BOX_COLOR.g, BOX_COLOR.b, BOX_COLOR.a);
        SDL_RenderFillRect(renderer, &inputBox);

        // Vẽ ô trạng thái
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &statusBox);
        SDL_SetRenderDrawColor(renderer, BOX_COLOR.r, BOX_COLOR.g, BOX_COLOR.b, BOX_COLOR.a);
        SDL_RenderFillRect(renderer, &statusBox);

        // Chuyển input text thành texture
        // Đảm bảo không render văn bản rỗng để tránh lỗi TTF_RenderText_Solid
        SDL_Surface* inputSurface = nullptr;
        if (!inputText.empty()) {
            inputSurface = TTF_RenderText_Solid(font, inputText.c_str(), TEXT_COLOR);
        } else {
            // Render một khoảng trắng nếu input rỗng để tránh lỗi nếu TTF_RenderText_Solid không thích chuỗi rỗng
            inputSurface = TTF_RenderText_Solid(font, " ", TEXT_COLOR);
        }
        
        SDL_Texture* inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
        SDL_Rect inputTextRect = { inputBox.x + 5, inputBox.y + 5, inputSurface->w, inputSurface->h };
        SDL_FreeSurface(inputSurface);

        // Chuyển status text thành texture
        SDL_Color statusColor = statusText == "OK" ? OK_COLOR : (statusText.empty() ? TEXT_COLOR : NOT_FOUND_COLOR);
        SDL_Surface* statusSurface = nullptr;
        if (!statusText.empty()) {
            statusSurface = TTF_RenderText_Solid(font, statusText.c_str(), statusColor);
        } else {
            statusSurface = TTF_RenderText_Solid(font, " ", statusColor);
        }

        SDL_Texture* statusTexture = SDL_CreateTextureFromSurface(renderer, statusSurface);
        SDL_Rect statusTextRect = { statusBox.x + 5, statusBox.y + 5, statusSurface->w, statusSurface->h };
        SDL_FreeSurface(statusSurface);

        // Vẽ text lên màn hình
        if (inputTexture) {
            SDL_RenderCopy(renderer, inputTexture, nullptr, &inputTextRect);
            SDL_DestroyTexture(inputTexture);
        }
        if (statusTexture) {
            SDL_RenderCopy(renderer, statusTexture, nullptr, &statusTextRect);
            SDL_DestroyTexture(statusTexture);
        }

        // Cập nhật màn hình
        SDL_RenderPresent(renderer);
    }

    // Dừng nhận sự kiện nhập liệu văn bản
    SDL_StopTextInput(); // <--- Đã thêm dòng này

    // Giải phóng tài nguyên
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}