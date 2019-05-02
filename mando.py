import socket
import sys
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
TOPIC_rot = "MANDO/rot"
TOPIC_modo = "MANDO/modo"

sense = SenseHat()
sense.rotation = 90
R = [255, 0, 0] #Red
W = [255, 255, 255] # White
N = [0, 0, 0] #none

img1= [[R, R, R, N, N, R, R, R, R, N, N, N, N, N, N, R, R, N, N, N, N, N, N, R, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, R, N, N, N, N, N, N, R, R, N, N, N, N, N, N, R, R, R, R, R, R, R, R, R], [ N, N, N, N, N, N, N, N, N, R, R, R, R, R, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, R, R, R, R, R, N, N, N, N, N, N, N, N, N], [N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, R, R, R, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, R, R, R, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N], [ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, R, R, N, N, N, N, N, N, R, R, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N]]

img2= [[N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, W, R, N, N, N, N, N, W, R, W, R, N, N, N, N, R, W, R, W, N, N, N, N, N, R, W, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N], [N, N, N, N, N, N, N, N, N, N, R, W, R, W, N, N, N, R, W, R, W, R, W, N, N, W, R, W, R, W, R, N, N, R, W, R, W, R, W, N, N, W, R, W, R, W, R, N, N, N, W, R, W, R, N, N, N, N, N, N, N, N, N, N], [N, N, R, W, R, W, N, N, N, R, W, R, W, R, W, N, R, W, R, W, R, W, R, W, W, R, W, R, W, R, W, R, R, W, R, W, R, W, R, W, W, R, W, R, W, R, W, R, N, W, R, W, R, W, R, N, N, N, W, R, W, R, N, N], [N, N, W, R, W, R, N, N, N, W, R, W, R, W, R, N, W, R, W, R, W, R, W, W, R, W, R, W, R, W, R, R, W, R, W, R, W, R, W, W, R, W, R, W, R, W, R, R, N, R, W, R, W, R, W, N, N, N, R, W, R, W, N, N]]

global estado
global num
global atino
global modo
x=0
y=0
estado=1
posX=0
posY=0
boton=0
atino=0
num = 9
modo=1

def on_log(client, userdata, level, buf):
	print("log: "+buf)
def on_connect(client, userdata, flags, rc):
	if rc==0:
		print("Connected OK")
def on_message(client, userdata, msg):
	global estado
	global num
	global atino
	global modo
	print(msg.topic+" "+str(msg.payload))
	if msg.topic==TOPIC_estado:
		estado=int(msg.payload,10)
	if msg.topic==TOPIC_num:
		num=int(msg.payload,10)
	if msg.topic==TOPIC_atino:
		atino=int(msg.payload,10)
	if msg.topic==TOPIC_rot:
		sense.rotation=int(msg.payload)
	if msg.topic==TOPIC_modo:
		modo=int(msg.payload,10)	
	
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.on_log = on_log

client.connect(host)
client.loop_start()

client.subscribe(TOPIC_estado)
client.subscribe(TOPIC_num)
client.subscribe(TOPIC_atino)
client.subscribe(TOPIC_rot)
client.subscribe(TOPIC_modo)

while True:
	if estado==1:
		sense.show_message("Bienvenido", text_colour=[255, 0, 0])
		estado=2
	elif estado==2:
		y = int(sense.get_accelerometer_raw()['x']*50)
		x = int(sense.get_accelerometer_raw()['y']*100)

		if modo==1:
			for event in sense.stick.get_events():
				if event.action == "pressed":
					if event.direction == "middle":
						boton=1
						client.publish(TOPIC_boton, "1")

		if (x>50):
			posX=50
			client.publish(TOPIC_posX, posX)
		elif (x<-50):
			posX=-50
			client.publish(TOPIC_posX, posX)
		elif ((x>posX+1) or (x<posX-1)):
			posX=x
			client.publish(TOPIC_posX, posX)
		if (y>100):
			posY=100
			client.publish(TOPIC_posY, posY)
		elif (y<0):
			posY=0
			client.publish(TOPIC_posY, posY)
		elif ((y>posY+1) or (y<posY-1)):
			posY=y
			client.publish(TOPIC_posY, posY)


		sense.clear()
		if num>0:
			sense.show_letter(str(num), text_colour=[255, 0, 0])
		elif num==-1:
			sense.show_message("You WIN", text_colour=[255, 0, 0])
			num=0

		if boton==1:
			boton=0
			for i in range (0, 4):
				sense.set_pixels(img1[i])
				time.sleep(0.4)
				sense.clear()

		if atino==1:
			atino=0
			print("Entro en atino")
			for i in range (0, 4):
				sense.set_pixels(img2[i])
				time.sleep(0.15)
				sense.clear()
	elif estado==0:
		sense.show_message("Chau!!!...", text_colour=[255, 0, 0])
		estado=-1
	time.sleep(.1)
	






