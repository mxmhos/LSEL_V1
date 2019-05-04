CC = gcc
CFLAGS = -Wall -g

all:main.c
	$(CC) $(CFLAGS) -o main main.c MQTT/mqtt.c -pthread -lpaho-mqtt3cs


clean:
	rm *.o main
