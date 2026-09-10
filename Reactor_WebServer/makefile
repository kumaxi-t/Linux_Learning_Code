CXX := g++
# 开启 C++14 支持 std::make_unique，添加头文件搜索路径 -Iinclude
CXXFLAGS := -std=c++17 -Wall -O2 -Iinclude -lpthread

# 搜索所有的 cpp 源码
SRCS := $(wildcard src/*.cpp)
# 将 .cpp 映射为 .o 目标文件
OBJS := $(SRCS:.cpp=.o)

# 最终可执行文件名
TARGET := epoll_server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f src/*.o $(TARGET)