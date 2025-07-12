#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        system("pause"); // Giữ terminal mở để xem lỗi
        return 1;
    }

    // Khởi tạo SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        system("pause");
        SDL_Quit();
        return 1;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        system("pause");
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("SDL2 Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        system("pause");
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        system("pause");
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tải hình ảnh nền
    SDL_Surface* backgroundSurface = IMG_Load("assets/image/ground.png");
    if (!backgroundSurface) {
        std::cout << "Unable to load image! SDL_image Error: " << IMG_GetError() << std::endl;
        system("pause");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);
    if (!backgroundTexture) {
        std::cout << "Unable to create texture! SDL_Error: " << SDL_GetError() << std::endl;
        system("pause");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tải font
    TTF_Font* font = TTF_OpenFont("assets/font/Pixel.ttf", 24);
    if (!font) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        system("pause");
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tạo văn bản
    SDL_Color textColor = {255, 255, 255, 255}; // Màu trắng
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Hello, Scrabble!", textColor);
    if (!textSurface) {
        std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        system("pause");
        TTF_CloseFont(font);
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (!textTexture) {
        std::cout << "Unable to create text texture! SDL_Error: " << SDL_GetError() << std::endl;
        system("pause");
        TTF_CloseFont(font);
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Vị trí văn bản
    SDL_Rect textRect = {WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 12, 200, 24};

    // Vòng lặp chính
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Xóa màn hình
        SDL_RenderClear(renderer);

        // Vẽ hình nền
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        // Vẽ văn bản
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Cập nhật màn hình
        SDL_RenderPresent(renderer);
    }

    // Giải phóng tài nguyên
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(backgroundTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}