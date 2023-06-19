LIB_NAME = myInterface

CFLAGS = -Wall -I ../myReadKey -I ../myTerm -I ../myBigChars -I ../mySimpleComputer

.PHONY: all

all: $(LIB_NAME).o
	ar cr lib$(LIB_NAME).a $(LIB_NAME).o

$(LIB_NAME).o:
	${CC} $(CFLAGS) -c $(LIB_NAME).c -o $(LIB_NAME).o