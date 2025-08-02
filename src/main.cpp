#include <iostream>
#include "core/Game.h"
#include "core/Board.h"
#include "core/Player.h"
#include "core/TileBag.h"
#include "core/Move.h"
#include "core/Tile.h"
#include <vector>
#include <string>

// Hàm trợ giúp để in bàn cờ ra console
void printBoard(const Board& board) {
    std::cout << "\n    0 1 2 3 4 5 6 7 8 9 A B C D E\n";
    std::cout << "    -------------------------------\n";
    for (int i = 0; i < Board::SIZE; ++i) {
        printf("%2d| ", i);
        for (int j = 0; j < Board::SIZE; ++j) {
            char letter = board.getTileLetter(i, j);
            std::cout << (letter == ' ' ? '.' : letter) << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

// Hàm trợ giúp để in khay chữ của người chơi
void printRack(const Player& player) {
    std::cout << player.getName() << "'s Rack: ";
    for (const Tile& tile : player.getRack()) {
        std::cout << tile.toString() << " ";
    }
    std::cout << "\n";
}


int main(int argc, char* argv[]) {
    try {
        // 1. Khởi tạo game
        Game game;
        std::cout << "--- Bat dau van choi moi voi 1 nguoi va 1 AI ---" << std::endl;
        game.startNewGame(1); // 1 người chơi (ID 0) và 1 AI (ID 1)

        // Lấy thông tin ban đầu
        const Player& humanPlayer = game.getPlayer(0);
        std::cout << "Nguoi choi hien tai: " << game.getPlayer(game.getCurrentPlayerId()).getName() 
                  << " (ID: " << game.getCurrentPlayerId() << ")" << std::endl;
        std::cout << "So luong o chu trong tui: " << game.getTileBag().remainingTiles() << std::endl;
        printRack(humanPlayer);

        // 2. Thử cho người chơi đặt một từ
        // LƯU Ý: Nước đi này có thể thất bại nếu người chơi không có đủ các chữ cái H, E, L, L, O.
        // Đây là một phần của việc kiểm tra logic game.
        std::cout << "\n--- Luot cua nguoi choi (ID 0) ---" << std::endl;
        std::cout << "Nguoi choi thu dat tu 'HELLO' tai trung tam (7, 7)." << std::endl;

        // Giả sử người chơi có các chữ cái cần thiết
        if (game.playWord(0, "HELLO", 7, 7, true)) {
            std::cout << "Dat tu 'HELLO' thanh cong!" << std::endl;
            std::cout << "Diem so moi cua " << humanPlayer.getName() << ": " << humanPlayer.getScore() << std::endl;
            printRack(humanPlayer); // In khay chữ đã được cập nhật
        } else {
            std::cout << "Dat tu 'HELLO' that bai. (Co the do khong du chu cai)." << std::endl;
            std::cout << "Nguoi choi bo luot." << std::endl;
            game.passTurn(0);
        }

        // Hiển thị bàn cờ sau nước đi của người chơi
        printBoard(game.getBoard());

        // 3. Kích hoạt lượt của AI
        std::cout << "\n--- Den luot cua AI (ID 1) ---" << std::endl;
        std::cout << "Nguoi choi hien tai: " << game.getPlayer(game.getCurrentPlayerId()).getName() 
                  << " (ID: " << game.getCurrentPlayerId() << ")" << std::endl;
        
        // Hàm nextTurn() sẽ tự động xử lý việc AI tạo và thực hiện nước đi
        game.nextTurn(); 
        
        std::cout << "AI da hoan thanh luot cua minh." << std::endl;
        const Player& aiPlayer = game.getPlayer(1);
        std::cout << "Diem so moi cua " << aiPlayer.getName() << ": " << aiPlayer.getScore() << std::endl;

        // Hiển thị bàn cờ sau khi AI chơi
        printBoard(game.getBoard());

        // 4. Kiểm tra trạng thái kết thúc game
        if (game.getState() == Game::State::GAME_OVER) {
            std::cout << "--- GAME OVER! ---" << std::endl;
        } else {
            std::cout << "--- Van choi tiep tuc... ---" << std::endl;
            std::cout << "Nguoi choi hien tai: " << game.getPlayer(game.getCurrentPlayerId()).getName() 
                      << " (ID: " << game.getCurrentPlayerId() << ")" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Da xay ra loi: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
