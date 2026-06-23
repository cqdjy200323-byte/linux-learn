CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2
LDFLAGS = 

SRCDIR = src
OBJDIR = obj
BINDIR = bin

# 自动找到所有 .cpp 文件
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
# 生成对应的 .o 文件
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
# 生成对应的可执行文件（每个 .cpp 生成同名可执行文件）
TARGETS = $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%,$(SOURCES))

all: $(TARGETS)

# 创建目录
$(OBJDIR) $(BINDIR):
	mkdir -p $@

# 通用编译规则
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 通用链接规则
$(BINDIR)/%: $(OBJDIR)/%.o | $(BINDIR)
	$(CXX) $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJDIR)/*.o $(TARGETS)

distclean: clean
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean distclean