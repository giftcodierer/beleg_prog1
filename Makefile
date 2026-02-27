# Belegarbeit Programmieren 1, WS2025/26, Materialverwaltung
# Name: Apel, Julian
# Matrikelnummer: 57849
# HTW-ID: s90353

#Mit "make" compiliert das ganze Projekt, mit "make <name>" compiliert nur die angegebene Datei

CC=gcc
CFLAGS=`pkg-config --cflags gtk+-3.0` -Wall
LIBS=`pkg-config --libs gtk+-3.0`

OBJS=main.o material.o fileio.o ui.o

all: beleg_prog1

beleg_prog1: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

main.o: main.c material.h fileio.h ui.h
	$(CC) $(CFLAGS) -c -o $@ main.c

material.o: material.c material.h
	$(CC) $(CFLAGS) -c -o $@ material.c

fileio.o: fileio.c fileio.h material.h
	$(CC) $(CFLAGS) -c -o $@ fileio.c

ui.o: ui.c ui.h material.h fileio.h
	$(CC) $(CFLAGS) -c -o $@ ui.c

clean:
	rm -f *.o beleg_prog1
