CC = gcc
OBJS = src/*.c
OBJ = bin/vector
CFLAGS = 
LIBS = 

all: run

build:
	@$(CC) $(OBJS) $(CFLAGS) $(LIBS) -o $(OBJ)

run: build
	@./$(OBJ)

clean:
	@rm ./$(OBJ)
	@echo "Cleaned!"
