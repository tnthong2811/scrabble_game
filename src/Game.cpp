#include "core/Game.h"
#include "AI/ScrabbleAI.h" 
#include <iostream>
#include <fstream>
#include <sstream>

Game::Game() : currentPlayerId_(0), state_(State::NOT_STARTED) {}

void Game::startNewGame(int aiCount) {
    board_.reset();
    tileBag_.reset();
    consecutivePasses_ = 0; // Reset bộ đếm bỏ lượt

    // Load từ điển một lần khi bắt đầu game
    // Hãy đảm bảo file "dictionary.txt" nằm ở đúng vị trí mà chương trình có thể đọc được
    if (!dictionary_.load("dictionary.txt")) {
        std::cerr << "LỖI NGHIÊM TRỌNG: Không thể tải file từ điển 'dictionary.txt'." << std::endl;
        state_ = State::GAME_OVER;
        return;
    }

    // Khởi tạo AI với độ khó mong muốn
    // Bạn có thể thay đổi độ khó ở đây (EASY, MEDIUM, HARD)
    ai_ = std::make_unique<AI::ScrabbleAI>(AI::Difficulty::HARD, dictionary_);

    setupPlayers(aiCount);
    for (auto& player : players_) {
        if(player) refillRack(*player);
    }

    state_ = State::PLAYING;
    currentPlayerId_ = 0;
}

void Game::setupPlayers(int aiCount) {
    players_.clear();
    players_.push_back(std::make_unique<Player>("Human"));

    for (int i = 0; i < aiCount; ++i) {
        players_.push_back(std::make_unique<Player>("AI " + std::to_string(i + 1))); 
    }
}

bool Game::playWord(int playerId, const std::string& word, int row, int col, bool horizontal) {
    if (state_ != State::PLAYING || playerId != currentPlayerId_) {
        // Không phải lượt của người chơi này
        return false;
    }

    Player* player = getPlayer(playerId);
    if (!player) return false;

    // Kiểm tra sơ bộ xem người chơi có đủ chữ cái không
    if (!player->canFormWord(word)) {
        std::cout << "Nước đi không hợp lệ: Bạn không có đủ chữ cái để tạo từ '" << word << "'." << std::endl;
        return false;
    }

    Move move(word, row, col, horizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL);

    // BƯỚC 1: Yêu cầu Board kiểm tra và tính điểm bằng logic mới
    MoveResult result = board_.validateAndScoreMove(move, dictionary_);

    // BƯỚC 2: Xử lý kết quả
    if (result.isValid) {
        std::cout << "Nước đi hợp lệ! Điểm số: " << result.score << std::endl;
        for(const auto& w : result.wordsFormed) {
            std::cout << " -> Từ được tạo: " << w << std::endl;
        }

        // BƯỚC 3: Thực thi nước đi lên bàn cờ thật
        board_.executeMove(move);

        // BƯỚC 4: Cập nhật trạng thái người chơi
        player->addScore(result.score);
        player->removeTilesFromRack(word);
        refillRack(*player);

        consecutivePasses_ = 0; // Reset bộ đếm bỏ lượt khi có nước đi thành công
        nextTurn();
        return true;
    } else {
        // Thông báo lỗi nếu nước đi không hợp lệ
        std::cout << "Nước đi không hợp lệ: " << result.errorMessage << std::endl;
        return false;
    }
}

void Game::nextTurn() {
    if (state_ != State::PLAYING) return;

    // Luôn kiểm tra điều kiện kết thúc game trước khi chuyển lượt
    if (checkGameEnd()) {
        calculateFinalScores();
        state_ = State::GAME_OVER;
        endGame();
        return;
    }

    currentPlayerId_ = (currentPlayerId_ + 1) % players_.size();

    // Nếu là lượt của AI, tự động xử lý
    // So sánh tên để xác định có phải AI hay không
    if (getPlayer(currentPlayerId_) && getPlayer(currentPlayerId_)->getName().rfind("AI", 0) == 0) {
        processAITurn();
    }
}

void Game::processAITurn() {
    Player* aiPlayer = getPlayer(currentPlayerId_);
    if (!aiPlayer) return;

    std::cout << "\n--- Đến lượt của " << aiPlayer->getName() << " ---" << std::endl;

    // AI vẫn dùng logic của nó để tìm nước đi tốt nhất
    // Lưu ý: Giả định ScrabbleAI có hàm generatePlay trả về đối tượng Play
    Play bestPlay = ai_->generatePlay(board_, aiPlayer->getRack());

    // Nếu AI quyết định bỏ lượt
    if (bestPlay.isPass()) {
        std::cout << aiPlayer->getName() << " đã bỏ lượt." << std::endl;
        passTurn(currentPlayerId_); // Gọi passTurn để xử lý
        return;
    }

    Move aiMove = bestPlay.getMove();
    std::cout << aiPlayer->getName() << " định đi từ '" << aiMove.getWord() << "'." << std::endl;

    // Game sẽ kiểm tra lại nước đi của AI bằng logic chuẩn của Board
    MoveResult result = board_.validateAndScoreMove(aiMove, dictionary_);

    if (result.isValid) {
        std::cout << "Nước đi của AI hợp lệ! Điểm thực tế: " << result.score << std::endl;
        board_.executeMove(aiMove);
        aiPlayer->addScore(result.score);
        aiPlayer->removeTilesFromRack(aiMove.getWord());
        refillRack(*aiPlayer);
        consecutivePasses_ = 0; // Reset bộ đếm bỏ lượt
    } else {
        // Nếu vì lý do nào đó nước đi của AI không hợp lệ, AI sẽ bị mất lượt
        std::cout << "Tuy nhiên, nước đi của AI không hợp lệ: " << result.errorMessage << ". AI bỏ lượt." << std::endl;
        consecutivePasses_++;
    }
    nextTurn();
}

bool Game::swapTiles(int playerId, const std::vector<char>& letters) {
    if (state_ != State::PLAYING || playerId != currentPlayerId_ || playerId != 0) {
        return false;
    }

    Player& player = *players_[playerId];
    if (player.swapTiles(tileBag_, letters)) {
        refillRack(player);
        return true;
    }
    return false;
}

void Game::passTurn(int playerId) {
    if (state_ != State::PLAYING || playerId != currentPlayerId_) return;
    consecutivePasses_++;
    std::cout << getPlayer(playerId)->getName() << " đã bỏ lượt." << std::endl;
    nextTurn();
}

void Game::refillRack(Player& player) {
    while (!player.isRackFull() && !tileBag_.isEmpty()) {
        player.addToRack(tileBag_.drawTile());
    }
}

bool Game::checkGameEnd() const {
    // Điều kiện 1: Túi hết chữ VÀ một người chơi hết chữ trên tay
    if (tileBag_.isEmpty()) {
        for (const auto& player : players_) {
            if (player && player->getRack().empty()) {
                std::cout << "Điều kiện kết thúc: Túi và khay chữ của một người chơi đã hết." << std::endl;
                return true;
            }
        }
    }
    // Điều kiện 2: Tất cả người chơi bỏ lượt 2 vòng liên tiếp
    if (consecutivePasses_ >= static_cast<int>(players_.size() * 2)) {
        std::cout << "Điều kiện kết thúc: Tất cả người chơi đã bỏ lượt 2 vòng." << std::endl;
        return true;
    }
    return false;
}

void Game::calculateFinalScores() {
    int bonusPoints = 0;
    Player* finisher = nullptr;

    // Trừ điểm của những người chơi còn chữ
    for (auto& player : players_) {
        if (!player) continue;
        int penalty = 0;
        if (!player->getRack().empty()) {
            for (const Tile& tile : player->getRack()) {
                penalty += tile.getValue();
            }
            player->addScore(-penalty); // Trừ điểm từ chính người chơi đó
            bonusPoints += penalty;     // Cộng dồn điểm phạt để thưởng cho người về nhất
        } else {
            // Tìm người đã hết chữ (nếu có)
            finisher = player.get();
        }
    }

    // Cộng điểm thưởng cho người hết chữ đầu tiên (nếu có)
    if (finisher) {
        finisher->addScore(bonusPoints);
    }
}

bool Game::saveGame(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) return false;

    file << "SCRIBBLE_SAVE_FILE\n";
    file << currentPlayerId_ << "\n";
    file << tileBag_.serialize() << "\n";
    board_.serialize(file);

    file << players_.size() << "\n";
    for (const auto& player : players_) {
        player->serialize(file);
    }

    return true;
}

bool Game::loadGame(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    std::string header;
    std::getline(file, header);
    if (header != "SCRIBBLE_SAVE_FILE") return false;

    file >> currentPlayerId_;

    std::string tileBagData;
    std::getline(file, tileBagData); 
    std::getline(file, tileBagData);
    tileBag_.deserialize(tileBagData);

    board_.deserialize(file);

    int playerCount;
    file >> playerCount;
    players_.resize(playerCount);
    for (int i = 0; i < playerCount; ++i) {
        players_[i] = std::make_unique<Player>();
        players_[i]->deserialize(file);
    }

    state_ = State::PLAYING;
    return true;
}

void Game::endGame() {
    state_ = State::GAME_OVER;
    std::cout << "Game has ended." << std::endl;
}
Game::State Game::getState() const { return state_; } 
const Board& Game::getBoard() const { return board_; }
Player* Game::getPlayer(int id) const {
    if (id >= 0 && id < static_cast<int>(players_.size())) {
        return players_[id].get(); // .get() trả về con trỏ thô từ unique_ptr
    }
    return nullptr; // Trả về nullptr nếu id không hợp lệ
}
int Game::getCurrentPlayerId() const { return currentPlayerId_; }
const TileBag& Game::getTileBag() const { return tileBag_; }