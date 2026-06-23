CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -I./include

# 目录定义
SRCDIR = src
BINDIR = bin

# 源文件（注意：不再包含 ThreadPool.cpp 和 Task.cpp）
SERVER_SRCS = $(SRCDIR)/Server.cpp
CLIENT_SRCS = $(SRCDIR)/Client.cpp

# 目标可执行文件
SERVER_TARGET = $(BINDIR)/server
CLIENT_TARGET = $(BINDIR)/client

# 默认目标
all: $(BINDIR) $(SERVER_TARGET) $(CLIENT_TARGET)

# 创建 bin 目录
$(BINDIR):
	mkdir -p $(BINDIR)

# 编译服务器（只编译 Server.cpp，头文件会被包含进来）
$(SERVER_TARGET): $(SERVER_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "✓ 服务器编译完成: $@"

# 编译客户端
$(CLIENT_TARGET): $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "✓ 客户端编译完成: $@"

# 清理
clean:
	rm -rf $(BINDIR)
	@echo "✓ 清理完成"

# 运行服务器
run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET) 8080

# 运行客户端
run-client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET) 127.0.0.1 8080

# 调试信息
debug:
	@echo "=== 文件检查 ==="
	@echo "Server.cpp: $(shell test -f src/Server.cpp && echo '存在' || echo '不存在')"
	@echo "Client.cpp: $(shell test -f src/Client.cpp && echo '存在' || echo '不存在')"
	@echo "ThreadPool.h: $(shell test -f include/ThreadPool.h && echo '存在' || echo '不存在')"
	@echo "Task.h: $(shell test -f include/Task.h && echo '存在' || echo '不存在')"
	@echo "Handler.h: $(shell test -f include/Handler.h && echo '存在' || echo '不存在')"

.PHONY: all clean run-server run-client debug