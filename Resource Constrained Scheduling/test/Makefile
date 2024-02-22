# Makefile for compiling m11215060.cpp with GLPK library

# 執行編譯的 C++ 編譯器
CXX = g++ -std=c++11

# 連接的庫 (-l)
LDFLAGS =  -lglpk
# 目標檔名和依賴的源文件
TARGET = m11215060
SOURCE = m11215060.cpp

# 預設目標
all: $(TARGET)

# 編譯目標
$(TARGET): $(SOURCE)
	$(CXX) -o $(TARGET) $(SOURCE) $(LDFLAGS)

# 清理編譯產生的中間檔
clean:
	rm -f $(TARGET)
