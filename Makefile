##### global settings #####

.PHONY: gdb test run submit clean

CC := g++
CFLAGS := -Wall -c -g -O2 -std=c++0x
CFLAGS_EXTRA := -ggdb3 -O2

SRC_DIR := src
INCLUDE_DIR := include
LIB_DIR := lib

LD_FLAGS := -lpcre++ -lpcre -lyaml-cpp

all: parser
OBJS := parser.o configloader.o $(LIB_DIR)/libuaparser.a parser
TARGET = parser
testcase := res

##### rules for building the project #####

parser: $(INCLUDE_DIR)/parser.h $(SRC_DIR)/bootstrap.cpp $(LIB_DIR)/libuaparser.a $(LIB_DIR)/libpcre++.a $(LIB_DIR)/libyaml-cpp.a
	$(CC) $(SRC_DIR)/bootstrap.cpp -o parser -I$(INCLUDE_DIR) -L$(LIB_DIR) -luaparser $(LD_FLAGS) -Wall -std=c++0x $(CFLAGS_EXTRA)

parser.o: $(INCLUDE_DIR)/debug.h $(INCLUDE_DIR)/common.h $(INCLUDE_DIR)/pcre++.h $(INCLUDE_DIR)/configloader.h $(SRC_DIR)/parser.cpp configloader.o
	$(CC) $(SRC_DIR)/parser.cpp $(CFLAGS) $(CFLAGS_EXTRA) -I$(INCLUDE_DIR) -L$(LIB_DIR) -lyaml-cpp
 
configloader.o: $(INCLUDE_DIR)/debug.h $(INCLUDE_DIR)/common.h $(INCLUDE_DIR)/configloader.h $(SRC_DIR)/configloader.cpp
	$(CC) $(SRC_DIR)/configloader.cpp $(CFLAGS) $(CFLAGS_EXTRA) -I$(INCLUDE_DIR) -L$(LIB_DIR) -lyaml-cpp

$(LIB_DIR)/libuaparser.a: parser.o configloader.o $(LIB_DIR)/libpcre++.a $(LIB_DIR)/libyaml-cpp.a
	-rm $(LIB_DIR)/libuaparser.a
	-ar crv $(LIB_DIR)/libuaparser.a parser.o configloader.o

##### rules for cleaning the project #####

clean:
	-rm -f $(OBJS) 2> /dev/null

##### some convinient rules #####

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	-cat $(testcase) | ./$(TARGET)

submit: clean
	git archive master -o ../user-agent-parser-cpp.zip
