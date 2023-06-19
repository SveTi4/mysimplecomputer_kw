LIB_NAME = myALU
CFLAGS = -Wall -I ../myALU -I ../myInterface -I ../myBigChars -I ../myTerm -I ../mySimpleComputer -I../myReadKey

.PHONY: all

all: $(LIB_NAME).o
	ar cr lib$(LIB_NAME).a $(LIB_NAME).o

$(LIB_NAME).o:
	${CC} $(CFLAGS) -c $(LIB_NAME).c -o $(LIB_NAME).o $(LDLIBS)