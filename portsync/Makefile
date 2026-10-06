CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Icpp/include
SANFLAGS = -fsanitize=address,undefined -g

CORE_SRC = cpp/src/Trade.cpp cpp/src/Holding.cpp cpp/src/NewsItem.cpp cpp/src/TradeList.cpp cpp/src/TickerTree.cpp cpp/src/Parser.cpp cpp/src/PortfolioManager.cpp cpp/src/Projection.cpp cpp/src/Dashboard.cpp cpp/src/SocketServer.cpp

MAIN_SRC = cpp/src/main.cpp $(CORE_SRC)

TEST_LIST_SRC = tests/cpp/test_list.cpp $(CORE_SRC)
TEST_AVL_SRC = tests/cpp/test_avl.cpp $(CORE_SRC)
TEST_PARSER_SRC = tests/cpp/test_parser.cpp $(CORE_SRC)
TEST_PORTFOLIO_SRC = tests/cpp/test_portfolio.cpp $(CORE_SRC)
TEST_PROJECTION_SRC = tests/cpp/test_projection.cpp $(CORE_SRC)

BIN_DIR = bin

all: prepare portsync test_list test_avl test_parser test_portfolio test_projection

prepare:
	@mkdir -p $(BIN_DIR)

portsync: $(MAIN_SRC) | prepare
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) -o portsync

test_list: $(TEST_LIST_SRC) | prepare
	$(CXX) $(CXXFLAGS) $(TEST_LIST_SRC) -o $(BIN_DIR)/test_list

test_avl: $(TEST_AVL_SRC) | prepare
	$(CXX) $(CXXFLAGS) $(TEST_AVL_SRC) -o $(BIN_DIR)/test_avl

test_parser: $(TEST_PARSER_SRC) | prepare
	$(CXX) $(CXXFLAGS) $(TEST_PARSER_SRC) -o $(BIN_DIR)/test_parser

test_portfolio: $(TEST_PORTFOLIO_SRC) | prepare
	$(CXX) $(CXXFLAGS) $(TEST_PORTFOLIO_SRC) -o $(BIN_DIR)/test_portfolio

test_projection: $(TEST_PROJECTION_SRC) | prepare
	$(CXX) $(CXXFLAGS) $(TEST_PROJECTION_SRC) -o $(BIN_DIR)/test_projection

sanitizer: CXXFLAGS += $(SANFLAGS)
sanitizer: clean all

test: test_list test_avl test_parser test_portfolio test_projection
	./$(BIN_DIR)/test_list
	./$(BIN_DIR)/test_avl
	./$(BIN_DIR)/test_parser
	./$(BIN_DIR)/test_portfolio
	./$(BIN_DIR)/test_projection

clean:
	rm -rf $(BIN_DIR) *.o portsync

.PHONY: all prepare portsync test clean sanitizer
