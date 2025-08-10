#include "core/Game.h"
#include "ui/GameUI.h"

#ifdef _WIN32
#undef main // Fix lỗi "unresolved external symbol _main" trên Windows
#endif

int main(int argc, char* argv[]) {
    Game scrabbleGame; // Tạo đối tượng logic game
    
    // *** THAY ĐỔI DUY NHẤT Ở ĐÂY ***
    // Khởi tạo ván chơi mới với 1 người chơi và 1 MÁY
    scrabbleGame.startNewGame(1); 

    GameUI scrabbleUI(scrabbleGame); // Tạo đối tượng giao diện
    scrabbleUI.run(); // Chạy vòng lặp game

    return 0;
}