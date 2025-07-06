CC = gcc
CFLAGS =
LFLAGS =
SRC = compact-regex_usage
FILETYPE = .c
	
$(SRC): $(SRC)$(FILETYPE)

	$(CC) $(CFLAGS) $(SRC)$(FILETYPE) $(LFLAGS) -o $(SRC)