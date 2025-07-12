#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    // Khởi tạo SDL, SDL_image và SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "SDL_Init thất bại: " << SDL_GetError() << endl;
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        cerr << "IMG_Init thất bại: " << IMG_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() == -1) {
        cerr << "TTF_Init thất bại: " << TTF_GetError() << endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Hiển thị ảnh và văn bản",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Tạo cửa sổ thất bại: " << SDL_GetError() << endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Tạo renderer thất bại: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load ảnh PNG thành texture
    SDL_Texture* image = IMG_LoadTexture(renderer, "ground.png");
    if (!image) {
        cerr << "Load ảnh thất bại: " << IMG_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo phông chữ
    TTF_Font* font = TTF_OpenFont("Pixel.ttf", 24);
    if (!font) {
        cerr << "Không thể mở phông chữ: " << TTF_GetError() << endl;
        SDL_DestroyTexture(image);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Vẽ văn bản
    SDL_Color textColor = {255, 255, 255}; // Màu trắng
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Hello, SDL!", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Vòng lặp chính
    bool running = true;
    SDL_Event event;
    while (running) {
        // Xử lý sự kiện
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Vẽ ảnh và văn bản lên màn hình
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, image, nullptr, nullptr); // Vẽ ảnh
        SDL_RenderCopy(renderer, textTexture, nullptr, nullptr); // Vẽ văn bản
        SDL_RenderPresent(renderer);
    }

    // Giải phóng
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}