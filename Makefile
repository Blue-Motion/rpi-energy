CC = gcc
CFLAGS = -Wall -pedantic
#OBJECTS = 

readserial: readserial.c
	$(CC) $(CFLAGS) readserial.c -o readserial
