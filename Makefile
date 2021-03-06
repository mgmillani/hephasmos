INSTALLDIR=$(HOME)/bin
CC = g++
CFLAGS = -Wall -Wextra -I$(INC) -D ERR_ON=1
SRC = src
OBJ = obj
BIN = bin
INC = include
LIB = -lboost_regex
OBJECTS = $(OBJ)/main.o $(OBJ)/directives.o $(OBJ)/addressings.o $(OBJ)/registers.o $(OBJ)/instructions.o $(OBJ)/machine.o $(OBJ)/labels.o $(OBJ)/memory.o $(OBJ)/stringer.o $(OBJ)/numbers.o $(OBJ)/sha1.o $(OBJ)/messenger.o $(OBJ)/expression.o $(OBJ)/multiExpression.o $(OBJ)/assembler.o $(OBJ)/file.o $(OBJ)/operands.o $(OBJ)/types.o
NAME = hephasmos

all: release obj

obj:
	mkdir obj

release: CFLAGS += -s -O2 -D TRACE_ON=0
release: assembler

debug: CFLAGS += -g -D TRACE_ON=1
debug: assembler

install: all
	cp "$(BIN)/$(NAME)" "$(INSTALLDIR)/$(NAME)"

assembler: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LIB) -o $(BIN)/$(NAME)

$(OBJ)/assembler.o: $(SRC)/assembler.cpp $(OBJ)/instructions.o $(OBJ)/registers.o $(OBJ)/labels.o $(OBJ)/machine.o $(OBJ)/directives.o
	$(CC) $(CFLAGS) $(SRC)/assembler.cpp -c -o $(OBJ)/assembler.o
$(OBJ)/instructions.o :$(SRC)/instructions.cpp $(OBJ)/stringer.o $(OBJ)/numbers.o $(OBJ)/registers.o $(OBJ)/labels.o
	$(CC) $(CFLAGS) $(SRC)/instructions.cpp -c -o $(OBJ)/instructions.o
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) $< -c -o $@
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f obj/*.o
	rm -f bin/$(NAME)
