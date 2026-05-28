BIN = code
CC = gcc
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
LFLAGS=-o
FLAGS=-c
RM=rm -f

$(BIN):$(OBJ)
	@$(CC) $(LFLAGS) $@ $^
	@echo "linking ... $^ to $@"
%.o:%.c
	@$(CC) $(FLAGS) $<
	@echo "compling ... $< to $@"
.PHONY:clean
clean:
	$(RM) $(OBJ) $(BIN)
