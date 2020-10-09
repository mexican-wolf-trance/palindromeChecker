CC = gcc
CFLAGS = -Wall -g -std=gnu99
TARGET = master
TARGET2 = palin
OBJ1 = main.o
OBJ2 = palindromeChecker.o
all: $(TARGET) $(TARGET2)
$(TARGET): $(OBJ1)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ1)
$(TARGET2): $(OBJ2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJ2)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
palindromeChecker.o: palindromeChecker.c
	$(CC) $(CFLAGS) -c palindromeChecker.c

