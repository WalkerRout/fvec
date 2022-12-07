CC = gcc
OBJS = src/*.c
OBJ = bin/vector
CFLAGS = -lm
LIBS = 

all: run

build:
	@$(CC) $(OBJS) $(CFLAGS) $(LIBS) -o $(OBJ)

run: build
	@./$(OBJ)

clean:
	@rm ./$(OBJ)
	@echo "Cleaned!"
