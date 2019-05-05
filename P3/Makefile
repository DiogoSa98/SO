# Makefile
# Sistemas Operativos, 2017-18

CFLAGS = -g -Wall -pthread
CC = gcc

heatSim: main.o matrix2d.o barrier.o
	$(CC) $(CFLAGS) -o heatSim main.o matrix2d.o barrier.o

main_maxD.o: main.c matrix2d.h barrier.h
	$(CC) $(CFLAGS) -pthread -c main.c

matrix2d.o: matrix2d.c matrix2d.h
	$(CC) $(CFLAGS) -c matrix2d.c

barrier.o: barrier.c barrier.h
	$(CC) $(CFLAGS) -c barrier.c

clean:
	rm -f *.o heatSim

zip:
	zip heatSim_p3.zip main.c matrix2d.c barrier.c matrix2d.h barrier.h Makefile

run:
	./heatSim 8 10.0 10.0 10.0 10.0 2032 2 5
