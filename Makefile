CC = gcc
CFLAGS = -Wall -g

all:main.c
	$(CC) $(CFLAGS) -o main main.c MQTT/mqtt.c topo/topo.c fsm.c tiempo.c -pthread -lpaho-mqtt3cs

test:publicadorMQTT.c
	$(CC) $(CFLAGS) -o test publicadorMQTT.c MQTT/mqtt.c topo/topo.c fsm.c tiempo.c -pthread -lpaho-mqtt3cs

clean:
	rm *.o main test
