CC=gcc
CFLAGS=-g -Wall -O3 
LDFLAGS=-lwiringPi -lrt -lcrypt -lm -lpaho-mqtt3c
#LDLIBS=-lwiringPi

all: piTankGo

#OBJS= fsm.o piTankGo_1.o player.o tmr.o torreta.o player.o cliente.o
OBJS= fsm.o piTankGo_1.o tmr.o torreta.o player.o mqtt.o

piTankGo: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	$(RM) *.o *~ piTankGo

