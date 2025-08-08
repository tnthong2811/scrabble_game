#pragma once
#include "BaseStrategy.h"
#include <vector>
#include <string>

// Forward declarations để tránh include không cần thiết
class Board;
class Tile;
class Move;

namespace AI {
namespace Strategies {

class MediumStrategy : public BaseStrategy {
public:
    /**
     * @brief Tạo ra một danh sách các nước đi (Move) tiềm năng.
     * Hàm này không đảm bảo các nước đi là hợp lệ 100% hay đã được tính điểm.
     */
    std::vector<Move> generatePlays(const Board& board, const std::vector<Tile>& rack) override;

private:
    // Tìm các vị trí có thể bắt đầu một nước đi (điểm neo)
    std::vector<std::pair<int, int>> findValidPositions(const Board& board);
    // Tạo ra các từ có thể có từ các chữ cái trên tay
    std::vector<std::string> generatePotentialWords(const std::vector<Tile>& rack, int maxLength);

    // CÁC HÀM CŨ ĐÃ BỊ XÓA:
    // - tryPlaceWord
    // - calculateScore
};

} // namespace Strategies
} // namespace AI