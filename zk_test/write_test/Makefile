CXX=g++
ZK_INC=/home/jingyu/zookeeper-3.4.6/src/c/build/include
ZK_LIB=/home/jingyu/zookeeper-3.4.6/src/c/build/lib
CXXFLAGS=-static -I $(ZK_INC) -L $(ZK_LIB) -lzookeeper_mt -g -O0 -pthread

SRC=main.cpp
BIN=test
all:
	$(CXX) $(SRC) $(CXXFLAGS) -o $(BIN)
    
