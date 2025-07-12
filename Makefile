# =========================================================================
# ===                 MAKEFILE CHO DỰ ÁN SDL2 TRÊN WINDOWS                ===
# =========================================================================

# --- 0. Sử dụng Command Prompt làm shell ---
SHELL = cmd.exe

# --- 1. Trình biên dịch ---
CXX = g++

# --- 2. Các thư mục của dự án ---
SRC_DIR     = src
INCLUDE_DIR = include
BUILD_DIR   = build
LIB_DIR     = lib
ASSETS_DIR  = assets

# --- 3. Tìm kiếm tất cả các file mã nguồn ---
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))

# --- 4. Tên file thực thi đầu ra ---
TARGET = $(BUILD_DIR)/main.exe

# --- 5. Cờ biên dịch và liên kết (Flags) ---
CXXFLAGS = -Wall -g -DSDL_MAIN_HANDLED \
           -I"$(INCLUDE_DIR)" \
           -I"$(LIB_DIR)/SDL2/include/SDL2" \
           -I"$(LIB_DIR)/SDL2_image/include/SDL2_image" \
           -I"$(LIB_DIR)/SDL2_ttf/include/SDL2_ttf"

LDFLAGS = -L"$(LIB_DIR)/SDL2/lib" \
          -L"$(LIB_DIR)/SDL2_image/lib" \
          -L"$(LIB_DIR)/SDL2_ttf/lib" \
          -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf \
          -mwindows

# --- 6. Danh sách các file DLL cần thiết ---
DLLS = $(LIB_DIR)/SDL2/bin/SDL2.dll \
       $(LIB_DIR)/SDL2_image/bin/SDL2_image.dll \
       $(LIB_DIR)/SDL2_ttf/bin/SDL2_ttf.dll

# =========================================================================
# ===                           CÁC QUY TẮC (RULES)                       ===
# =========================================================================

# --- Quy tắc mặc định: "all" ---
all: $(TARGET) copy_dlls

# --- Quy tắc liên kết: Tạo file .exe từ các file .o ---
$(TARGET): $(OBJ)
	@echo Linking...
	$(CXX) -o $@ $^ $(LDFLAGS)
	@echo Build successful! Target is at $(TARGET)

# --- Quy tắc biên dịch: Tạo file .o từ các file .cpp ---
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo Compiling $<...
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Quy tắc sao chép DLLs vào thư mục build ---
copy_dlls: $(DLLS)
	@echo Copying DLLs to $(BUILD_DIR)...
	@copy "$(LIB_DIR)\SDL2\bin\SDL2.dll" "$(BUILD_DIR)"
	@copy "$(LIB_DIR)\SDL2_image\bin\SDL2_image.dll" "$(BUILD_DIR)"
	@copy "$(LIB_DIR)\SDL2_ttf\bin\SDL2_ttf.dll" "$(BUILD_DIR)"
	@echo DLLs copied successfully!

# --- Quy tắc dọn dẹp: "clean" ---
clean:
	@echo Cleaning build files...
	@if exist "$(BUILD_DIR)" del /Q "$(BUILD_DIR)\*.o"
	@if exist "$(TARGET)" del /Q "$(TARGET)"
	@if exist "$(BUILD_DIR)" del /Q "$(BUILD_DIR)\*.dll"
	@echo Done.

# --- Quy tắc PHONY ---
.PHONY: all clean copy_dlls