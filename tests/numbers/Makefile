CC = g++
INCLUDE = ../../include
CFLAGS = -Wall -Wextra -g -I$(INCLUDE)
SRC = ../../src
OBJ = .
BIN = .
OBJECTS = $(OBJ)/numbers.o
NAME = readnum

all: $(OBJECTS)
	# $(CC) $(CFLAGS) $(OBJECTS) readNum.cpp -o readnum
	$(CC) $(CFLAGS) $(OBJECTS) autotest.cpp -o autotest	

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) $< -c -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f *.o
	rm -f $(NAME)
