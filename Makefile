CC=gcc
CFLAGS=`pkg-config --cflags gtk+-3.0` -Wall
LIBS=`pkg-config --libs gtk+-3.0`

OBJS=main.o material.o fileio.o ui.o

all: beleg_prog1

beleg_prog1: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *.o beleg_prog1
