CXX=g++
CXXFLAGS=-std=c++17 -O2 -I./modules/cpu -I./modules/mem -I./modules/io
TARGET=kernel
SRCS=cli/main.cpp modules/cpu/rr_scheduler.cpp modules/mem/lru.cpp modules/io/sync.cpp
all:
\t$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)
clean:
\trm -f $(TARGET)
