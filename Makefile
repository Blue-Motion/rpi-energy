CC = gcc
CFLAGS = -Wall -pedantic
#OBJECTS = 

#readserial: readserial.c
#	$(CC) $(CFLAGS) readserial.c -o readserial

reader: getValue.c
	$(CC) $(CFLAGS) getValue.c -o reader
