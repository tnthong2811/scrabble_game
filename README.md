# Game Scrabble với AI thông minh 🎯

![Contributors](https://img.shields.io/github/contributors/tnthong2811/scrabble_game)
![Commits](https://img.shields.io/github/commit-activity/m/tnthong2811/scrabble_game)
![License](https://img.shields.io/github/license/tnthong2811/scrabble_game)
![Languages](https://img.shields.io/github/languages/count/tnthong2811/scrabble_game)
![Top Language](https://img.shields.io/github/languages/top/tnthong2811/scrabble_game)


---

## Giới thiệu

Project này là một bản cài đặt hoàn chỉnh của game Scrabble, từ giao diện người dùng, luật chơi, hệ thống tính điểm cho đến một AI đủ thông minh để trở thành một đối thủ đáng gờm. Đặc biệt, AI ở cấp độ "Hard" sử dụng cấu trúc dữ liệu GADDAG nâng cao để tìm ra các nước đi tối ưu, mang lại trải nghiệm đầy thử thách.

---

## ✨ Tính năng chính

* **Giao diện đồ họa đầy đủ:** Menu chính, màn hình chơi game, và màn hình kết thúc được thiết kế bằng SDL2.
* **3 cấp độ AI:**
    * **Easy:** AI tìm các từ đơn giản và đặt ở vị trí ngẫu nhiên.
    * **Medium:** AI tìm các nước đi kết nối với các từ có sẵn trên bàn cờ.
    * **Hard:** AI sử dụng thuật toán GADDAG để tìm ra tất cả các nước đi có thể và chọn nước đi có điểm số cao nhất.
* **Hệ thống gợi ý:** Gợi ý cho người chơi 5 nước đi tốt nhất tại mỗi lượt.
* **Tính điểm tự động:** Tự động tính điểm cho các từ chính và các từ phụ được tạo ra (cross-words).
* **Luật chơi đầy đủ:** Bao gồm các ô thưởng (Double/Triple Letter/Word), luật chơi cho lượt đầu tiên, và các điều kiện kết thúc game.
* **Tối ưu hóa hiệu năng:** AI "Hard" sử dụng file dữ liệu GADDAG đã được biên dịch trước (`gaddag.dat`) để khởi động game gần như tức thì.

---

## 🛠️ Công nghệ sử dụng

* **Ngôn ngữ:** C++ (phiên bản C++17 trở lên)
* **Thư viện đồ họa:**
    * SDL2
    * SDL2_image (để tải ảnh)
    * SDL2_ttf (để hiển thị font chữ)
* **Hệ thống Build:** Make / Makefile
* **Cấu trúc dữ liệu chính:** Trie (Từ điển), GADDAG (AI Hard)

---

## 🚀 Hướng dẫn cài đặt và chạy game

### 1. Yêu cầu

Trước khi bắt đầu, hãy đảm bảo bạn đã cài đặt các công cụ sau:
* Trình biên dịch C++ hỗ trợ C++17 (ví dụ: g++ trên MinGW/Linux, Clang trên macOS).
* Công cụ `make`.

### 2. Cài đặt thư viện SDL2

Tôi đã tải sẵn các thư viện SDL2 đưa lên repo của mình, bạn không cần phải tải thủ công.

### 3. Biên dịch Project

Mở terminal hoặc cửa sổ dòng lệnh trong thư mục gốc của project và chạy các lệnh sau theo thứ tự:

1.  **(Chỉ chạy một lần duy nhất)** Biên dịch và chạy công cụ tạo file GADDAG cho AI "Hard":
    ```bash
    make build_gaddag
    ```
    Sau đó chạy công cụ:
    ```bash
    # Trên Windows
    .\build\build_gaddag.exe

    # Trên Linux/macOS
    ./build/build_gaddag
    ```
    Lệnh này sẽ tạo ra file `assets/gaddag.dat`.
    Những hướng dẫn trên bạn chỉ cần chạy duy nhất 1 lần.
2.  Biên dịch game chính:
    ```bash
    make
    ```

### 4. Chạy game

Sau khi biên dịch thành công, bạn có thể khởi động game bằng lệnh:
```bash
# Trên Windows
build\game.exe

# Trên Linux/macOS
./build/game
```

---

## 🎮 Cách chơi (Usage)
- **Bắt đầu game mới:** AI sẽ đi trước (có thể thay đổi trong file `Game.cpp`).
- **Lượt của bạn:** Kéo thả các quân cờ từ khay (phía dưới) lên bàn cờ, sau đó nhấn **SUBMIT**.
- **Nước đi không hợp lệ:** Các quân cờ sẽ tự động quay về khay, thông báo lỗi sẽ hiện trong console.
- **Lượt của AI:** AI sẽ tự động chơi, bạn có thể xem chi tiết nước đi (từ đã đặt, điểm số) trong console.
- **Từ điển:** Bạn có thể thêm từ mới vào file `assets/dictionary/dictionary.txt`.
- **Thoát game:** Đóng cửa sổ trò chơi.

---

## 🤝 Đóng góp (Contributing)

Mọi sự đóng góp đều được chào đón! Nếu bạn có ý tưởng để cải thiện game, vui lòng tạo một "Pull Request" hoặc mở một "Issue" để chúng ta cùng thảo luận.

1.  Fork the Project
2.  Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3.  Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4.  Push to the Branch (`git push origin feature/AmazingFeature`)
5.  Open a Pull Request

---

## 📜 Giấy phép

Project này được cấp phép theo Giấy phép MIT. Xem file `LICENSE` để biết thêm chi tiết.

---

## 🙏 Lời cảm ơn

* Cảm ơn thư viện SDL đã cung cấp một nền tảng tuyệt vời để phát triển game 2D.
