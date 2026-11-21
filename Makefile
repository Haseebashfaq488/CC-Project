CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Isrc/frontend
FLEX = flex
BISON = bison

BIN_DIR = bin
SRC_DIR = src
FRONTEND_DIR = src/frontend

TARGET = $(BIN_DIR)/yaarc

PARSER_SRC = $(FRONTEND_DIR)/parser.tab.cpp
PARSER_HDR = $(FRONTEND_DIR)/parser.tab.hpp
LEXER_SRC = $(FRONTEND_DIR)/lex.yy.cpp

SRCS = $(SRC_DIR)/main.cpp $(PARSER_SRC) $(LEXER_SRC)

all: $(TARGET)

$(PARSER_SRC) $(PARSER_HDR): $(FRONTEND_DIR)/parser.y
	$(BISON) -d -o $(PARSER_SRC) $(FRONTEND_DIR)/parser.y

$(LEXER_SRC): $(FRONTEND_DIR)/lexer.l $(PARSER_HDR)
	$(FLEX) -o $(LEXER_SRC) $(FRONTEND_DIR)/lexer.l

$(TARGET): $(PARSER_SRC) $(LEXER_SRC) $(SRC_DIR)/main.cpp
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

test: $(TARGET)
	@echo "\n=== Running Test Suite on Valid Programs ==="
	@for test_file in tests/valid/*.yaar; do \
		echo "\n----------------------------------------"; \
		echo "Running: $$test_file"; \
		./$(TARGET) $$test_file || exit 1; \
	done
	@echo "\n=== Running Test Suite on Invalid Programs ==="
	@for test_file in tests/invalid/*.yaar; do \
		echo "\n----------------------------------------"; \
		echo "Running: $$test_file"; \
		./$(TARGET) $$test_file || true; \
	done
	@echo "\n>>> All Tests Completed! <<<"

clean:
	rm -rf $(BIN_DIR) $(PARSER_SRC) $(PARSER_HDR) $(LEXER_SRC)

.PHONY: all clean test
