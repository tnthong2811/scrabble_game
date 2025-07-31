#pragma once
#include "Tile.h"
#include <vector>
#include <random>
#include <map>
#include <memory>
#include <string>

class TileBag {
public:
    // === Constructors ===
    TileBag();

    // === Tile Management ===
    /**
     * @brief Draws a single tile from the bag.
     * @return The drawn tile.
     * @throws std::runtime_error if the bag is empty.
     */
    Tile drawTile();

    /**
     * @brief Draws multiple tiles from the bag.
     * @param count Number of tiles to draw.
     * @return Vector of drawn tiles.
     * @throws std::runtime_error if count exceeds remaining tiles.
     */
    std::vector<Tile> drawTiles(int count);

    /**
     * @brief Returns a tile to the bag.
     * @param tile The tile to return.
     */
    void returnTile(Tile tile);

    /**
     * @brief Returns multiple tiles to the bag.
     * @param tiles Vector of tiles to return.
     */
    void returnTiles(const std::vector<Tile>& tiles);

    // === Bag Operations ===
    void shuffle();                        // Trộn túi tile
    void reset();                          // Reset về trạng thái ban đầu

    // === State Queries ===
    bool isEmpty() const;                  // Kiểm tra túi rỗng
    int remainingTiles() const;            // Số tile còn lại
    int initialTileCount() const;          // Tổng số tile ban đầu

    // === Statistical Info ===
    std::map<char, int> getLetterDistribution() const; // Phân phối chữ cái
    int getRemainingLetterCount(char letter) const;     // Số lượng chữ cụ thể

    // === Serialization ===
    std::string serialize() const;         // Chuyển trạng thái thành chuỗi
    bool deserialize(const std::string& data); // Khôi phục từ chuỗi

private:
    std::vector<Tile> tiles_;              // Các tile trong túi
    std::vector<Tile> initialSet_;         // Bộ tile ban đầu (để reset)
    std::mt19937 rng_;                     // Bộ sinh số ngẫu nhiên
    std::map<char, int> letterCounts_;     // Đếm số lượng tile theo ký tự

    void initializeStandardSet();          // Khởi tạo bộ tile chuẩn
    void initializeFromSet(const std::vector<Tile>& set); // Khởi tạo từ bộ tùy chỉnh
    void updateLetterCounts(const Tile& tile, int delta); // Cập nhật số đếm tile
};

// Cấu hình tile chuẩn cho Scrabble tiếng Anh
namespace TileDistributions {
    const std::vector<Tile> STANDARD_ENGLISH = {
        // Blank tiles (2)
        Tile(' ', 0, true), Tile(' ', 0, true),
        // A-9, B-2, C-2, D-4, E-12, F-2, G-3, H-2, I-9, J-1, K-1, L-4, M-2,
        Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1),
        Tile('A', 1), Tile('A', 1), Tile('A', 1), Tile('A', 1),
        Tile('B', 3), Tile('B', 3),
        Tile('C', 3), Tile('C', 3),
        Tile('D', 2), Tile('D', 2), Tile('D', 2), Tile('D', 2),
        Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1),
        Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1), Tile('E', 1),
        Tile('E', 1), Tile('E', 1),
        Tile('F', 4), Tile('F', 4),
        Tile('G', 2), Tile('G', 2), Tile('G', 2),
        Tile('H', 4), Tile('H', 4),
        Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1),
        Tile('I', 1), Tile('I', 1), Tile('I', 1), Tile('I', 1),
        Tile('J', 8),
        Tile('K', 5),
        Tile('L', 1), Tile('L', 1), Tile('L', 1), Tile('L', 1),
        Tile('M', 3), Tile('M', 3),
        // N-6, O-8, P-2, Q-1, R-6, S-4, T-6, U-4, V-2, W-2, X-1, Y-2, Z-1
        Tile('N', 1), Tile('N', 1), Tile('N', 1), Tile('N', 1), Tile('N', 1), Tile('N', 1),
        Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1), Tile('O', 1),
        Tile('O', 1), Tile('O', 1), Tile('O', 1),
        Tile('P', 3), Tile('P', 3),
        Tile('Q', 10),
        Tile('R', 1), Tile('R', 1), Tile('R', 1), Tile('R', 1), Tile('R', 1), Tile('R', 1),
        Tile('S', 1), Tile('S', 1), Tile('S', 1), Tile('S', 1),
        Tile('T', 1), Tile('T', 1), Tile('T', 1), Tile('T', 1), Tile('T', 1), Tile('T', 1),
        Tile('U', 1), Tile('U', 1), Tile('U', 1), Tile('U', 1),
        Tile('V', 4), Tile('V', 4),
        Tile('W', 4), Tile('W', 4),
        Tile('X', 8),
        Tile('Y', 4), Tile('Y', 4),
        Tile('Z', 10)
    };

    // Có thể thêm bộ tile cho ngôn ngữ khác (ví dụ: tiếng Việt)
    const std::vector<Tile> VIETNAMESE = {
        // Thêm phân phối cho tiếng Việt tại đây...
    };
}