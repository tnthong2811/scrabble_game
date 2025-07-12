SDL2_INC = C:/scrabble_game/SDL2-devel-2.30.1-mingw/SDL2-2.30.1/x86_64-w64-mingw32/include
SDL2_LIB = C:/scrabble_game/SDL2-devel-2.30.1-mingw/SDL2-2.30.1/x86_64-w64-mingw32/lib
SDL2IMG_INC = C:/scrabble_game/SDL2_image-devel-2.8.2-mingw/SDL2_image-2.8.2/x86_64-w64-mingw32/include
SDL2IMG_LIB = C:/scrabble_game/SDL2_image-devel-2.8.2-mingw/SDL2_image-2.8.2/x86_64-w64-mingw32/lib
SDL2TTF_INC = C:/scrabble_game/SDL2_ttf-devel-2.19.3-mingw/SDL2_ttf-2.19.3/x86_64-w64-mingw32/include
SDL2TTF_LIB = C:/scrabble_game/SDL2_ttf-devel-2.19.3-mingw/SDL2_ttf-2.19.3/x86_64-w64-mingw32/lib
CXX = g++
CXXFLAGS = -I"$(SDL2_INC)" -I"$(SDL2_INC)/SDL2" -I"$(SDL2IMG_INC)" -I$(SDL2TTF_INC)
LDFLAGS = -mconsole -L"$(SDL2_LIB)" -L"$(SDL2IMG_LIB)" -L"$(SDL2TTF_LIB)" -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

TARGET = main.exe
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	del /Q $(TARGET)
