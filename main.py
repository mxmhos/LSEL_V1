import socket
import sys
import select
import time
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import paho.mqtt.subscribe as subscribe
from sense_hat import SenseHat

host = "192.168.1.202" # '172.16.2.3' #
port = 1883
TOPIC_estado = "MANDO/estado"
TOPIC_posX = "MANDO/posicionX"
TOPIC_posY = "MANDO/posicionY"
TOPIC_boton = "MANDO/boton"
TOPIC_num = "MANDO/numero"
TOPIC_atino = "MANDO/atino"
TOPIC_modo = "MANDO/modo"
TOPIC_rot = "MANDO/rot"


estado=1
posX=0
posY=0
boton=0
atino=0
num = 9
rot = 90
remain=1

def on_log(client, userdata, level, buf):
	print("log: "+buf)
def on_connect(client, userdata, flags, rc):
	if rc==0:
		print("Connected OK")
def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.payload))
	if msg.topic==TOPIC_posX:
		posX=int(msg.payload)
		print("Recibo posX: "+str(posX));
	if msg.topic==TOPIC_posY:
		posY=int(msg.payload)
		print("Recibo posY: "+str(posY));
	if msg.topic==TOPIC_boton:
		boton=int(msg.payload)
		print("Recibo boton: "+str(boton));


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.on_log = on_log

client.connect(host)
client.loop_start()

client.subscribe(TOPIC_posX)
client.subscribe(TOPIC_posY)
client.subscribe(TOPIC_boton)


input_expected = [sys.stdin]

while remain:
	input_read, output_read, exception_read = select.select(input_expected, [], [])
	for s in input_read:
		if s == sys.stdin:
			teclado = sys.stdin.readline()
			lista=teclado.split(' ')
			cod=lista[0]
			if cod == 'r':
				client.publish(TOPIC_rot, int(lista[1]))
			elif cod == 'n':
				if ((int(lista[1])>=-1) and (int(lista[1])<=9)):
					client.publish(TOPIC_num, lista[1])
			if teclado.strip() == 'a':
				client.publish(TOPIC_atino, "1")	
			elif teclado.strip() == 'md':
				client.publish(TOPIC_modo, "1")
			elif teclado.strip() == 'mf':
				client.publish(TOPIC_modo, "0")
			elif teclado.strip() == 'on':
				client.publish(TOPIC_estado, "1")
			elif teclado.strip() == 'off':
				client.publish(TOPIC_estado, "0")
			elif teclado.strip() == 'q':
				remain = 0







