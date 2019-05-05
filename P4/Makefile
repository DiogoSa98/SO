# Makefile, versao 3
# Sistemas Operativos, DEI/IST/ULisboa 2017-18

CC       = gcc
CFLAGS   = -g -std=gnu99 -Wall -pedantic -pthread

.PHONY: all clean zip

all: heatSim_p4

heatSim_p4: p3_main.o matrix2d.o util.o
	$(CC) $(CFLAGS) -o $@ $+

p3_main.o: p3_main.c matrix2d.h util.h
	$(CC) $(CFLAGS) -o $@ -c $<

matrix2d.o: matrix2d.c matrix2d.h
	$(CC) $(CFLAGS) -o $@ -c $<

util.o: util.c
	$(CC) $(CFLAGS) -o $@ -c $<

run: 
	./heatSim_p4 10 10 10 10 10 50000 5 0 teste 1
 
clean:
	rm -f *.o heatSim_p4

zip: heatSim_p4_solucao.zip

heatSim_p4_solucao.zip: Makefile p3_main.c matrix2d.h util.h matrix2d.c matrix2d.h util.c
	zip $@ $+

