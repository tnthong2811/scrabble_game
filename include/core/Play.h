#pragma once
#include "Move.h" // Giả định Move.h đã include Tile.h

class Play {
public:
    // === Constructors & Creators ===

    /**
     * @brief Constructor cho một nước đi (MOVE) cụ thể.
     */
    Play(const Move& move) : move_(move), isPass_(false) {}

    /**
     * @brief Hàm tĩnh để tạo một lượt bỏ qua (PASS).
     * Đây là cách duy nhất và rõ ràng để tạo ra một lượt bỏ qua.
     */
    static Play createPass() {
        // Gọi constructor private để tạo một Play object với isPass_ = true
        return Play(true);
    }


    // === Public Methods ===

    /**
     * @brief Kiểm tra xem đây có phải là một lượt bỏ qua hay không.
     * HÀM NÀY SỬA LỖI BIÊN DỊCH GỐC.
     */
    bool isPass() const {
        return isPass_;
    }

    /**
     * @brief Lấy đối tượng Move của nước đi.
     * Lưu ý: Không nên gọi hàm này nếu isPass() là true.
     */
    const Move& getMove() const {
        return move_;
    }

private:
    // Constructor private, chỉ được gọi bởi createPass() để đảm bảo tính nhất quán.
    Play(bool isPass) : move_(Move()), isPass_(isPass) {}

    Move move_;      // Chứa thông tin nước đi nếu không phải là pass
    bool isPass_;    // Cờ để xác định đây là lượt đi hay bỏ lượt
};