#include "core/Game.h"
#include "ui/GameUI.h"
#include <windows.h>

#ifdef _WIN32
#undef main // Fix lỗi "unresolved external symbol _main" trên Windows
#endif

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    Game scrabbleGame;
    GameUI scrabbleUI(scrabbleGame); // Tạo đối tượng giao diện
    scrabbleUI.run(); // Chạy vòng lặp game
    return 0;
}