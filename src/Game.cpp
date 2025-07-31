#include "core/Game.h"
#include "AI/ScrabbleAI.h" // Đảm bảo tích hợp với ScrabbleAI
#include <fstream>
#include <sstream>

Game::Game() : currentPlayerId_(0), state_(State::NOT_STARTED) {}

void Game::startNewGame(int aiCount) {
    if (state_ != State::NOT_STARTED || aiCount < 1) {
        endGame();
        return;
    }

    board_.reset();
    tileBag_.reset();

    setupPlayers(aiCount);

    // Deal initial tiles
    for (auto& player : players_) {
        refillRack(*player);
    }

    state_ = State::PLAYING;
    currentPlayerId_ = 0; // Bắt đầu với người chơi (ID 0 là human)
}

void Game::setupPlayers(int aiCount) {
    players_.clear();
    players_.push_back(std::make_unique<Player>("Human")); // Người chơi cố định ID 0

    for (int i = 0; i < aiCount; ++i) {
        players_.push_back(std::make_unique<Player>("AI " + std::to_string(i + 1))); // Giả sử AIPlayer kế thừa Player
    }
}

bool Game::playWord(int playerId, const std::string& word, int row, int col, bool horizontal) {
    if (state_ != State::PLAYING || playerId != currentPlayerId_ || playerId != 0) { // Chỉ nhân cho phép chơi
        return false;
    }

    Player& player = *players_[playerId];
    Move::Direction dir = horizontal ? Move::Direction::HORIZONTAL : Move::Direction::VERTICAL;
    Move move(word, row, col, dir);

    if (board_.placeWord(word, row, col, horizontal)) { // Sử dụng placeWord thay placeMove
        // Tính điểm (cần triển khai trong Board để bao gồm từ phụ)
        int score = board_.calculateWordScore(word, row, col, horizontal);
        move.setScore(score);

        // Remove used tiles from player's rack
        for (const Tile& tile : player.getRack()) {
            for (char c : word) {
                if (tile.getLetter() == c || (tile.isBlank() && tile.getValue() == 0)) {
                    player.removeFromRack(c);
                    move.addTileUsed(tile);
                    break;
                }
            }
        }

        player.addScore(score);
        refillRack(player);
        return true;
    }

    return false;
}

void Game::nextTurn() {
    if (state_ != State::PLAYING) return;

    currentPlayerId_ = (currentPlayerId_ + 1) % players_.size();

    if (checkGameEnd()) {
        calculateFinalScores();
        state_ = State::GAME_OVER;
    } else if (currentPlayerId_ != 0) { // Nếu là lượt AI
        processAITurn();
    }
}

void Game::processAITurn() {
    Player& aiPlayer = *players_[currentPlayerId_];
    AI::ScrabbleAI ai(AI::Difficulty::MEDIUM, dictionary_); // Sử dụng constructor đúng với Difficulty
    Play aiPlay = ai.generatePlay(board_, aiPlayer.getRack()); // Sử dụng board_ và rack từ aiPlayer

    if (board_.placeWord(aiPlay.getMove().getWord(), aiPlay.getMove().getRow(), aiPlay.getMove().getCol(),
                         aiPlay.getMove().getDirection() == Move::Direction::HORIZONTAL)) {
        // Áp dụng nước đi nếu hợp lệ
        playWord(currentPlayerId_, aiPlay.getMove().getWord(), aiPlay.getMove().getRow(), aiPlay.getMove().getCol(),
                 aiPlay.getMove().getDirection() == Move::Direction::HORIZONTAL);
    } else {
        passTurn(currentPlayerId_);
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
    nextTurn();
}

void Game::refillRack(Player& player) {
    while (!player.isRackFull() && !tileBag_.isEmpty()) {
        player.addToRack(tileBag_.drawTile());
    }
}

bool Game::checkGameEnd() const {
    if (tileBag_.isEmpty()) {
        for (const auto& player : players_) {
            if (!player->getRack().empty()) return true;
        }
    }
    return false;
}

void Game::calculateFinalScores() {
    for (auto& player : players_) {
        int penalty = 0;
        for (const Tile& tile : player->getRack()) {
            penalty += tile.getValue();
        }
        player->addScore(-penalty);
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
    std::getline(file, tileBagData); // Đọc dòng trống
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

Game::State Game::getState() const { return state_; } // Sử dụng Game::State để tránh lỗi
const Board& Game::getBoard() const { return board_; }
const Player& Game::getPlayer(int id) const { return *players_[id]; }
int Game::getCurrentPlayerId() const { return currentPlayerId_; }
const TileBag& Game::getTileBag() const { return tileBag_; }