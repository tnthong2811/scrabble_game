#include "core/Game.h"
#include "AI/ScrabbleAI.h" 
#include <iostream>
#include <fstream>
#include <sstream>

Game::Game() : currentPlayerId_(0), state_(State::NOT_STARTED), consecutivePasses_(0) {}

void Game::startNewGame(int aiCount) {
    board_.reset();
    tileBag_.reset();
    consecutivePasses_ = 0;
    if (!dictionary_.load("assets/dictionary/dictionary.txt")) {
        std::cerr << "LỖI: Không thể tải từ điển." << std::endl;
        state_ = State::GAME_OVER; return;
    }
    ai_ = std::make_unique<AI::ScrabbleAI>(AI::Difficulty::MEDIUM, dictionary_);
    setupPlayers(aiCount);
    for (auto& player : players_) { if(player) refillRack(*player); }
    state_ = State::PLAYING;
    currentPlayerId_ = 1; // AI đi trước
}

bool Game::playWord(int playerId, const std::string& wordFromRack, const std::string& fullWord, int row, int col, bool horizontal) {
    if (state_ != State::PLAYING || playerId != currentPlayerId_) return false;
    Player* player = getPlayer(playerId);
    if (!player) return false;

    Move move(fullWord, row, col, horizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL);
    MoveResult result = board_.validateAndScoreMove(move, *player, dictionary_);

    if (result.isValid) {
        board_.executeMove(move);
        player->addScore(result.score);
        player->removeTilesFromRack(result.lettersUsedFromRack);
        refillRack(*player);
        consecutivePasses_ = 0;
        nextTurn();
        return true;
    } else {
        std::cout << "Nuoc di khong hop le: " << result.errorMessage << std::endl;
        return false;
    }
}

void Game::processAITurn() {
    Player* aiPlayer = getPlayer(currentPlayerId_);
    if (!aiPlayer) return;
    std::cout << "\n--- Den luot cua " << aiPlayer->getName() << " ---" << std::endl;

    Play bestPlay = ai_->generatePlay(board_, aiPlayer->getRack());
    if (bestPlay.isPass()) {
        passTurn(currentPlayerId_); return;
    }
    
    Move aiMove = bestPlay.getMove();
    MoveResult result = board_.validateAndScoreMove(aiMove, *aiPlayer, dictionary_);

    if (result.isValid) {
        std::cout << "Nuoc di cua AI hop le! Tu: '" << aiMove.getWord() << "'. Diem: " << result.score << std::endl;
        board_.executeMove(aiMove);
        aiPlayer->addScore(result.score);
        aiPlayer->removeTilesFromRack(result.lettersUsedFromRack);
        refillRack(*aiPlayer);
        consecutivePasses_ = 0;
    } else {
        std::cout << "Nuoc di cua AI khong hop le: " << result.errorMessage << ". AI bo luot." << std::endl;
        consecutivePasses_++;
    }
    nextTurn();
}

void Game::nextTurn() {
    if (state_ != State::PLAYING) return;
    if (checkGameEnd()) {
        calculateFinalScores();
        state_ = State::GAME_OVER;
        return;
    }
    currentPlayerId_ = (currentPlayerId_ + 1) % players_.size();
}

void Game::update() {
    if (state_ == State::PLAYING && currentPlayerId_ != 0) {
        processAITurn();
    }
}

void Game::setupPlayers(int aiCount) {
    players_.clear();
    players_.push_back(std::make_unique<Player>("Human"));

    for (int i = 0; i < aiCount; ++i) {
        players_.push_back(std::make_unique<Player>("AI " + std::to_string(i + 1))); 
    }
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