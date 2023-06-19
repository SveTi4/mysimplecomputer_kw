LIB_NAME = mySimpleComputer
CFLAGS = -Wall

.PHONY: all

all: $(LIB_NAME).o
	ar cr lib$(LIB_NAME).a $(LIB_NAME).o

$(LIB_NAME).o:
	gcc $(CFLAGS) -c $(LIB_NAME).c -o $(LIB_NAME).o