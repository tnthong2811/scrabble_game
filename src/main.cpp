#include "core/Game.h"
#include "core/Board.h"
#include "core/Player.h"
#include "core/TileBag.h"
#include "core/Move.h"
#include "AI/ScrabbleAI.h" // Giả sử có file này cho AI
#include <iostream>
#include <vector>

int main() {
    // Khởi tạo game
    Game game;
    std::cout << "Starting a new game with 1 AI opponent...\n";
    game.startNewGame(1); // 1 người chơi và 1 AI

    // Lấy thông tin ban đầu
    std::cout << "Game state: " << static_cast<int>(game.getState()) << "\n";
    std::cout << "Current player ID: " << game.getCurrentPlayerId() << "\n";
    std::cout << "Tile bag size: " << game.getTileBag().getRemainingTiles() << "\n";

    // Hiển thị khay của người chơi
    const Player& player = game.getPlayer(0);
    std::cout << "Player rack: ";
    for (const Tile& tile : player.getRack()) {
        std::cout << tile.toString() << " ";
    }
    std::cout << "\n";

    // Test đặt từ
    std::string word = "HELLO";
    int row = 7, col = 7; // Đặt từ ở trung tâm
    bool horizontal = true;
    if (game.playWord(0, word, row, col, horizontal)) {
        std::cout << "Successfully placed word: " << word << "\n";
        std::cout << "Score: " << player.getScore() << "\n";
        std::cout << "Updated rack: ";
        for (const Tile& tile : player.getRack()) {
            std::cout << tile.toString() << " ";
        }
        std::cout << "\n";
    } else {
        std::cout << "Failed to place word: " << word << "\n";
    }

    // Hiển thị bảng sau khi đặt từ
    const Board& board = game.getBoard();
    std::cout << "Board state:\n";
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            char letter = board.getTileLetter(i, j);
            std::cout << (letter == ' ' ? '.' : letter) << " ";
        }
        std::cout << "\n";
    }

    // Test đổi tile
    std::vector<char> lettersToSwap = {'A', 'B'};
    if (game.swapTiles(0, lettersToSwap)) {
        std::cout << "Successfully swapped tiles: ";
        for (char c : lettersToSwap) {
            std::cout << c << " ";
        }
        std::cout << "\n";
        std::cout << "Updated rack: ";
        for (const Tile& tile : player.getRack()) {
            std::cout << tile.toString() << " ";
        }
        std::cout << "\n";
    } else {
        std::cout << "Failed to swap tiles\n";
    }

    // Test bỏ lượt
    std::cout << "Passing turn...\n";
    game.passTurn(0);
    std::cout << "New current player ID: " << game.getCurrentPlayerId() << "\n";

    // Kiểm tra lượt AI
    std::cout << "Simulating AI turn...\n";
    game.nextTurn(); // AI sẽ chơi lượt của nó

    // Hiển thị bảng sau lượt AI
    std::cout << "Board state after AI turn:\n";
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            char letter = board.getTileLetter(i, j);
            std::cout << (letter == ' ' ? '.' : letter) << " ";
        }
        std::cout << "\n";
    }

    // Kiểm tra trạng thái game
    if (game.getState() == Game::State::GAME_OVER) {
        std::cout << "Game Over!\n";
        for (int i = 0; i < game.getPlayers().size(); ++i) {
            std::cout << game.getPlayer(i).getName() << " score: " << game.getPlayer(i).getScore() << "\n";
        }
    }

    return 0;
}