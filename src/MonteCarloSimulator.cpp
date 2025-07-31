#include "AI/Utils/MonteCarloSimulator.h"
#include "AI/ScrabbleAI.h"

namespace AI {
namespace Utils {

MonteCarloSimulator::MonteCarloSimulator(const Dictionary& dictionary)
    : dictionary_(dictionary), simulationCount_(100) {}

SimulationResult MonteCarloSimulator::simulatePlay(const Play& play,
                                                 const Board& board,
                                                 const std::vector<Tile>& aiRack,
                                                 const std::vector<Tile>& opponentRack,
                                                 const TileBag& tileBag) {
    SimulationResult result;
    
    for (int i = 0; i < simulationCount_; i++) {
        // Tạo bản sao của trạng thái game
        Board simBoard = cloneAndApplyPlay(board, play);
        TileBag simBag = tileBag;
        std::vector<Tile> simAIRack = aiRack;
        std::vector<Tile> simOpponentRack = opponentRack;
        
        // Loại bỏ các tile đã sử dụng
        for (const Tile& tile : play.tilesUsed) {
            auto it = std::find(simAIRack.begin(), simAIRack.end(), tile);
            if (it != simAIRack.end()) {
                simAIRack.erase(it);
            }
        }
        
        // Rút thêm tile nếu cần
        int tilesToDraw = Player::RACK_SIZE - simAIRack.size();
        if (tilesToDraw > 0) {
            auto newTiles = simBag.drawTiles(tilesToDraw);
            simAIRack.insert(simAIRack.end(), newTiles.begin(), newTiles.end());
        }
        
        // Tạo AI cho đối thủ
        AI::ScrabbleAI opponentAI(AI::Difficulty::MEDIUM, dictionary_);
        
        // Mô phỏng game đến hết
        bool aiTurn = false;
        int aiScore = play.score;
        int opponentScore = 0;
        
        while (!simBag.isEmpty() && (!simAIRack.empty() || !simOpponentRack.empty())) {
            if (aiTurn) {
                Play aiPlay = opponentAI.generatePlay(simBoard, simAIRack);
                aiScore += aiPlay.score;
                // ... cập nhật board và rack
            } else {
                Play opponentPlay = opponentAI.generatePlay(simBoard, simOpponentRack);
                opponentScore += opponentPlay.score;
                // ... cập nhật board và rack
            }
            
            aiTurn = !aiTurn;
        }
        
        // Cập nhật kết quả
        result.aiScore += aiScore;
        result.opponentScore += opponentScore;
        if (aiScore > opponentScore) {
            result.winningProbability += 1.0f;
        }
    }
    
    // Chuẩn hóa kết quả
    result.aiScore /= simulationCount_;
    result.opponentScore /= simulationCount_;
    result.winningProbability /= simulationCount_;
    
    return result;
}

} // namespace Utils
} // namespace AI