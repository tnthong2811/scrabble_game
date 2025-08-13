#include "core/Game.h"
#include "AI/ScrabbleAI.h" 
#include <iostream>
#include <fstream>
#include <sstream>

Game::Game() : currentPlayerId_(1), state_(State::NOT_STARTED), consecutivePasses_(0) {}

void Game::startNewGame(int aiCount, int gameDurationMinutes, AI::Difficulty difficulty) {
    board_.reset();
    tileBag_.reset();
    consecutivePasses_ = 0;
    consecutiveSwaps_ = 0;
    
    totalGameTimeRemaining_ = gameDurationMinutes * 60 * 1000; 
    currentTurnTimeRemaining_ = 60 * 1000;    
    lastUpdateTime_ = SDL_GetTicks();         

    if (!dictionary_.load("assets/dictionary/dictionary.txt")) {
        std::cerr << "LỖI: Không thể tải từ điển." << std::endl;
        state_ = State::GAME_OVER; return;
    }
    ai_ = std::make_unique<AI::ScrabbleAI>(difficulty, dictionary_);
    setupPlayers(aiCount);
    for (auto& player : players_) { if(player) refillRack(*player); }
    state_ = State::PLAYING;
    currentPlayerId_ = 1; 
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
        turnHistory_.push_back({player->getName(), fullWord, result.score, false, false});
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

    std::vector<Play> topPlays = ai_->generateTopPlays(board_, aiPlayer->getRack(), 300);
    bool move_successful = false;

    for (const auto& play : topPlays) {
        if (play.isPass()) {
            continue;
        }

        Move aiMove = play.getMove();
        MoveResult result = board_.validateAndScoreMove(aiMove, *aiPlayer, dictionary_);

        if (result.isValid) {
            std::cout << "Nuoc di cua AI hop le! Tu: '" << aiMove.getWord() << "'. Diem: " << result.score << std::endl;
            board_.executeMove(aiMove);
            aiPlayer->addScore(result.score);
            aiPlayer->removeTilesFromRack(result.lettersUsedFromRack);
            refillRack(*aiPlayer);
            consecutivePasses_ = 0;
            turnHistory_.push_back({aiPlayer->getName(), aiMove.getWord(), result.score, false, false});
            move_successful = true;
            break; 
        } else {
             std::cout << "AI's suggested move '" << aiMove.getWord() << "' is invalid. Trying next..." << std::endl;
        }
    }

    if (!move_successful) {
        if (consecutiveSwaps_ < 1 && !tileBag_.isEmpty()) { 
            std::vector<char> toSwap = aiPlayer->selectSwapTiles();
            if (!toSwap.empty()) {
                std::cout << "AI swap tiles: ";
                for (char c : toSwap) std::cout << c << " ";
                std::cout << std::endl;
                bool swapSuccess = swapTiles(currentPlayerId_, toSwap);
                if (swapSuccess) {
                    consecutiveSwaps_++;
                    consecutivePasses_ = 0;
                    return;
                }
            }
        }
        std::cout << "AI khong tim thay nuoc di hop le nao. Bo luot." << std::endl;
        passTurn(currentPlayerId_);
        consecutiveSwaps_ = 0;  
        return; 
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
    currentTurnTimeRemaining_ = 60 * 1000;

    if (currentPlayerId_ == 0) {
        Player* humanPlayer = getPlayer(0);
        if (humanPlayer) {
            currentSuggestions_ = ai_->generateTopPlays(board_, humanPlayer->getRack(), 5);
        }
    } else {
        currentSuggestions_.clear();
    }
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
    if (state_ != State::PLAYING || playerId != currentPlayerId_) return false;  

    Player& player = *players_[playerId];
    if (player.swapTiles(tileBag_, letters)) {
        turnHistory_.push_back({player.getName(), "", 0, false, true});
        refillRack(player);
        nextTurn();
        return true;
    }
    return false;
}

void Game::passTurn(int playerId) {
    if (state_ != State::PLAYING || playerId != currentPlayerId_) return;
    consecutivePasses_++;
    turnHistory_.push_back({getPlayer(playerId)->getName(), "", 0, true, false});
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

int Game::getWinnerId() const {
    if (players_.empty()) return -1;

    int winnerId = -1;
    int maxScore = -9999;

    for (size_t i = 0; i < players_.size(); ++i) {
        if (players_[i] && players_[i]->getScore() > maxScore) {
            maxScore = players_[i]->getScore();
            winnerId = i;
        }
    }

    // Kiểm tra trường hợp hòa
    int countMaxScore = 0;
    for (const auto& player : players_) {
        if (player && player->getScore() == maxScore) {
            countMaxScore++;
        }
    }

    return (countMaxScore > 1) ? -1 : winnerId;
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
const std::vector<TurnRecord>& Game::getTurnHistory() const {
    return turnHistory_;
}

const std::vector<Play>& Game::getSuggestions() const {
    return currentSuggestions_;
}

void Game::updateTimers() {
    if (state_ != State::PLAYING) return;

    Uint32 currentTime = SDL_GetTicks();
    Uint32 deltaTime = currentTime - lastUpdateTime_;
    lastUpdateTime_ = currentTime;

    if (totalGameTimeRemaining_ > deltaTime) {
        totalGameTimeRemaining_ -= deltaTime;
    } else {
        totalGameTimeRemaining_ = 0;
    }

    if (currentTurnTimeRemaining_ > deltaTime) {
        currentTurnTimeRemaining_ -= deltaTime;
    } else {
        currentTurnTimeRemaining_ = 0;
    }

    if (totalGameTimeRemaining_ == 0) {
        std::cout << "Hết giờ! Trò chơi kết thúc." << std::endl;
        endGame();
        return;
    }
    if (currentTurnTimeRemaining_ == 0) {
        std::cout << getPlayer(currentPlayerId_)->getName() << " hết giờ! Tự động bỏ lượt." << std::endl;
        passTurn(currentPlayerId_);
    }
}

Uint32 Game::getTotalTimeRemaining() const {
    return totalGameTimeRemaining_;
}

Uint32 Game::getTurnTimeRemaining() const {
    return currentTurnTimeRemaining_;
}