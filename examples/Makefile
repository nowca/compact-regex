CC = gcc
CFLAGS = -I../include/
SRC1 = compact-regex_examples
SRC2 = compact-regex_file-reading
FILETYPE = .c

SRC = $(SRC1)$(FILETYPE) $(SRC2)$(FILETYPE)
BIN = $(patsubst %.c,%,$(SRC))

all: $(BIN)