import socket
import sys
import select
import time
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import paho.mqtt.subscribe as subscribe
from sense_hat import SenseHat

host = "192.168.1.202" # '172.16.2.4' # '172.16.2.3' #
port = 1883
TOPIC_estado = "estado"

TOPIC_posX = "MANDO/x"
TOPIC_posY = "MANDO/y"
TOPIC_start = "MANDO/start"
TOPIC_stop = "MANDO/stop"
TOPIC_disparo = "MANDO/disparo"

TOPIC_boton = "DISPLAY/boton"
TOPIC_rot = "DISPLAY/rot"

TOPIC_num = "puntuacion"
TOPIC_timer = "tiempo"

TOPIC_atino1 = "TOPO1/acierto"
TOPIC_servo1 = "TOPO1/posicion"
TOPIC_atino2 = "TOPO2/acierto"
TOPIC_servo2 = "TOPO2/posicion"
TOPIC_atino3 = "TOPO3/acierto"
TOPIC_servo3 = "TOPO3/posicion"

estado=0
posX=0
posY=0
start=0
stop=0
disparo=0
boton=0
rot=90
num = 9
timer=0
a1=0
a2=0
a3=0
t1=0
t2=0
t3=0

remain=1

def on_log(client, userdata, level, buf):
	print("log: "+buf)
def on_connect(client, userdata, flags, rc):
	if rc==0:
		print("Connected OK")
def on_message(client, userdata, msg):
	#print(msg.topic+" "+str(msg.payload))
	if msg.topic==TOPIC_posX:
		posX=int(msg.payload)
		print("Recibo posX: "+str(posX));
	if msg.topic==TOPIC_posY:
		posY=int(msg.payload)
		print("Recibo posY: "+str(posY));
	if msg.topic==TOPIC_start:
		start=int(msg.payload)
		print("Recibo start: "+str(start));
	if msg.topic==TOPIC_stop:
		stop=int(msg.payload)
		print("Recibo stop: "+str(stop));
	if msg.topic==TOPIC_disparo:
		disparo=int(msg.payload)
		print("Recibo disparo: "+str(disparo));
	if msg.topic==TOPIC_boton:
		boton=int(msg.payload)
		print("Recibo boton: "+str(boton));
	if msg.topic==TOPIC_rot:
		boton=int(msg.payload)
		print("Recibo rot: "+str(boton));
	if msg.topic==TOPIC_num:
		num=int(msg.payload)
		print("Recibo num: "+str(num));
	if msg.topic==TOPIC_timer:
		timer=int(msg.payload)
		print("Recibo timer: "+str(timer));
	if msg.topic==TOPIC_estado:
		estado=int(msg.payload)
		print("Recibo estado: "+str(estado));
	if msg.topic==TOPIC_atino1:
		a1=int(msg.payload)
		print("Recibo acerto1: "+str(a1));
	if msg.topic==TOPIC_atino2:
		a2=int(msg.payload)
		print("Recibo acerto2: "+str(a2));
	if msg.topic==TOPIC_atino3:
		a3=int(msg.payload)
		print("Recibo acerto3: "+str(a3));
	if msg.topic==TOPIC_servo1:
		t1=int(msg.payload)
		print("Recibo servo1: "+str(t1));
	if msg.topic==TOPIC_servo2:
		t2=int(msg.payload)
		print("Recibo servo1: "+str(t2));
	if msg.topic==TOPIC_servo3:
		t3=int(msg.payload)
		print("Recibo servo1: "+str(t3));


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
#client.on_log = on_log

client.connect(host)
client.loop_start()

client.subscribe(TOPIC_estado)

client.subscribe(TOPIC_posX)
client.subscribe(TOPIC_posY)
client.subscribe(TOPIC_start)
client.subscribe(TOPIC_stop)
client.subscribe(TOPIC_disparo)

client.subscribe(TOPIC_boton)
client.subscribe(TOPIC_rot)

client.subscribe(TOPIC_num)
client.subscribe(TOPIC_timer)

client.subscribe(TOPIC_atino1)
client.subscribe(TOPIC_servo1)
client.subscribe(TOPIC_atino2)
client.subscribe(TOPIC_servo2)
client.subscribe(TOPIC_atino3)
client.subscribe(TOPIC_servo3)


input_expected = [sys.stdin]

print("Opciones de simulacion:")
print("estado del sistema: e -valor- (0-1-2-3-4)")
print("senal de Start: m s (espacio final)")
print("senal de Stop: m p (espacio final)")
print("senal de Disparo: m d (espacio final)")
print("posX: m x -valor- (-50 a 50)")
print("posY: m y -valor- (0 a 100)")
print("rotacion: r -valor- (90-180-270-360)")
print("puntuacion: p -valor- (0 a 8)")
print("puntuacion: t -valor- (0 a 99)")
print("estado topo1: t1 -valor- (0/1)")
print("estado topo2: t2 -valor- (0/1)")
print("estado topo3: t3 -valor- (0/1)")
print("acierto topo1: a1 (espacio final)")
print("acierto topo2: a2 (espacio final)")
print("acierto topo3: a3 (espacio final)")


while remain:
	input_read, output_read, exception_read = select.select(input_expected, [], [])
	for s in input_read:
		if s == sys.stdin:
			teclado = sys.stdin.readline()
			lista=teclado.split(' ')
			cod=lista[0]
			if (cod == "e"):
				if ((int(lista[1])>=0) and (int(lista[1])<=4)):
					print("Publico estado")
					client.publish(TOPIC_estado, lista[1])
				else:
					print("Revise argumentos\n")
			elif (cod == "m"):
				if (lista[1] == "s"):
 					client.publish(TOPIC_start, "1")
					print("Publico start")
				elif (lista[1] == "p"):
 					client.publish(TOPIC_stop, "1")
					print("Publico stop")
				elif (lista[1] == "d"):
 					client.publish(TOPIC_disparo, "1")
					print("Publico disparo")
				elif (lista[1] == "x"):
					client.publish(TOPIC_posX, lista[2])
					print("Publico posX")
				elif (lista[1] == "y"):
					client.publish(TOPIC_posY, lista[2])
					print("Publico posY")
			elif (cod == "r"):
				client.publish(TOPIC_rot, int(lista[1]))
				print("Publico rot")
			elif (cod == "p"):
				if ((int(lista[1])>=-1) and (int(lista[1])<=9)):
					client.publish(TOPIC_num, lista[1])
			elif (cod == "t"):
				if ((int(lista[1])>=0) and (int(lista[1])<=99)):
					client.publish(TOPIC_timer, lista[1])
					print("Publico timer")
			elif (cod == "a1"):
				client.publish(TOPIC_atino1, "1")
				print("Publico TOPIC_atino1")
			elif (cod == "a2"):
				client.publish(TOPIC_atino2, "1")
				print("Publico TOPIC_atino2")
			elif (cod == "a3"):
				client.publish(TOPIC_atino3, "1")
				print("Publico TOPIC_atino3")	
			elif (cod ==  "t1"):
				if ((int(lista[1])>=0) and (int(lista[1])<=1)):
					client.publish(TOPIC_servo1, lista[1])
					print("Publico TOPIC_servo1")
			elif (cod == "t2"):
				if ((int(lista[1])>=0) and (int(lista[1])<=1)):
					client.publish(TOPIC_servo2, lista[1])
					print("Publico TOPIC_servo2")
			elif (cod == "t3"):
				if ((int(lista[1])>=0) and (int(lista[1])<=1)):
					client.publish(TOPIC_servo3, lista[1])
					print("Publico TOPIC_servo3")
			elif teclado.strip() == 'q':
				remain = 0






