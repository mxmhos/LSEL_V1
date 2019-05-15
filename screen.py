import socket
import sys
import pygame
import time
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import paho.mqtt.subscribe as subscribe
from sense_hat import SenseHat

#***************************************************************************
#***********   DEFINICIONES DE LOS TOPICS Y COMMS  *************************

host = "192.168.1.202" # '172.16.2.3' #
port = 1883
TOPIC_estado = "JUEGO/CONTROL"
TOPIC_posX = "MANDO/posicionX"
TOPIC_posY = "MANDO/posicionY"
TOPIC_boton = "MANDO/boton"
TOPIC_num = "MANDO/numero"
TOPIC_atino = "TOPO/sonido"
TOPIC_rot = "MANDO/rot"
TOPIC_modo = "JUEGO/modo"
TOPIC_timer = "MANDO/timer"
TOPIC_disparo = "MANDO/disparo"

#***************************************************************************
#***********   FUNCIONES Y DEFINICIONES DEL SCREEN *************************

R = [255, 0, 0] #Red
W = [255, 255, 255] # White
N = [0, 0, 0] #none
msgC1 = [255, 0, 0] #Red	TODOS
msgC2 = [255, 255, 255] #Red
msgC3 = [255, 0, 0] #Red

OFFSET_LEFT = 1
OFFSET_TOP = 3
NUMS =[1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,  # 0
       0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,  # 1
       1,1,1,0,0,1,0,1,0,1,0,0,1,1,1,  # 2
       1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,  # 3
       1,0,0,1,0,1,1,1,1,0,0,1,0,0,1,  # 4
       1,1,1,1,0,0,1,1,1,0,0,1,1,1,1,  # 5
       1,1,1,1,0,0,1,1,1,1,0,1,1,1,1,  # 6
       1,1,1,0,0,1,0,1,0,1,0,0,1,0,0,  # 7
       1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,  # 8
       1,1,1,1,0,1,1,1,1,0,0,1,0,0,1]  # 9

# Displays a single digit (0-9)
def show_digit(val, xd, yd, r, g, b):
  offset = val * 15
  for p in range(offset, offset + 15):
    xt = p % 3
    yt = (p-offset) // 3
    sense.set_pixel(xt+xd, yt+yd, r*NUMS[p], g*NUMS[p], b*NUMS[p])

# Displays a two-digits positive number (0-99)
def show_timer(val, r, g, b):
  abs_val = abs(val)
  tens = abs_val // 10
  units = abs_val % 10
  if (abs_val > 9): show_digit(tens, OFFSET_LEFT, OFFSET_TOP, r, g, b)
  show_digit(units, OFFSET_LEFT+4, OFFSET_TOP, r, g, b)

# Displays los atinos (0-8)
def show_vidas(val, rgb1, rgb2):
	for p in range(0, val):
		sense.set_pixel(7-p, 0, rgb1)
		sense.set_pixel(7-p, 1, rgb1)
	for p in range(val, 8):
		sense.set_pixel(7-p, 0, rgb2)
		sense.set_pixel(7-p, 1, rgb2)

imgDisparo= [[R, R, R, N, N, R, R, R, R, N, N, N, N, N, N, R, R, N, N, N, N, N, N, R, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, R, N, N, N, N, N, N, R, R, N, N, N, N, N, N, R, R, R, R, R, R, R, R, R], [ N, N, N, N, N, N, N, N, N, R, R, R, R, R, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, R, R, R, R, R, N, N, N, N, N, N, N, N, N], [N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, R, R, R, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, R, R, R, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N], [ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, R, R, N, N, N, N, N, N, R, R, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N]]

imgAtino= [[N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, W, R, N, N, N, N, N, W, R, W, R, N, N, N, N, R, W, R, W, N, N, N, N, N, R, W, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N], [N, N, N, N, N, N, N, N, N, N, R, W, R, W, N, N, N, R, W, R, W, R, W, N, N, W, R, W, R, W, R, N, N, R, W, R, W, R, W, N, N, W, R, W, R, W, R, N, N, N, W, R, W, R, N, N, N, N, N, N, N, N, N, N], [N, N, R, W, R, W, N, N, N, R, W, R, W, R, W, N, R, W, R, W, R, W, R, W, W, R, W, R, W, R, W, R, R, W, R, W, R, W, R, W, W, R, W, R, W, R, W, R, N, W, R, W, R, W, R, N, N, N, W, R, W, R, N, N], [N, N, W, R, W, R, N, N, N, W, R, W, R, W, R, N, W, R, W, R, W, R, W, W, R, W, R, W, R, W, R, R, W, R, W, R, W, R, W, W, R, W, R, W, R, W, R, R, N, R, W, R, W, R, W, N, N, N, R, W, R, W, N, N]]

imgGo= [N, N, N, N, N, N, N, N,R, R, R, R, N, N, R, N,R, N, N, N, N, R, N, R,R, N, N, N, N, R, N, R,R, N, R, R, N, R, N, R,R, N, N, R, N, R, N, R,R, N, N, R, N, R, N, R,N, R, R, N, N, N, R, N]


#***************************************************************************
#***********   TODA LA MOVIDA DE COMUNICACIONES    *************************
global estado
global num
global atino
global modo
global timer
global disparo
estado=-2
disparo=0
atino=0
num = 8
modo=1
timer=0
t_rst=0
seconds=0

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
	global timer
	global disparo
	print(msg.topic+" "+str(msg.payload))
	if msg.topic==TOPIC_estado:
		estado=int(msg.payload,10)
	if msg.topic==TOPIC_num:
		if ((num>=0) and (num<=8)):
			num=int(msg.payload,10)
	if msg.topic==TOPIC_atino:
		atino=int(msg.payload,10)
	if msg.topic==TOPIC_rot:
		sense.rotation=int(msg.payload)
	if msg.topic==TOPIC_modo:
		modo=int(msg.payload,10)
	if msg.topic==TOPIC_timer:
		t_rst=int(msg.payload,10)
	if msg.topic==TOPIC_disparo:
		disparo=int(msg.payload,10)

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
client.subscribe(TOPIC_timer)
client.subscribe(TOPIC_disparo)



#***************************************************************************
#***********          PROGRAMA PRINCIPAL           *************************
sense = SenseHat()
pygame.mixer.init()
sense.rotation = 90

while True:
	if estado==-2:		#ESTADO DE INICIO
		estado=1		#este '1' esta para que arranque SOLO!!!
		#estado=-1		#hay que poner estado en -1 si queremos que espere al MAIN!!!
		sense.show_message("Bienvenido", text_colour=msgC1)
		pygame.mixer.music.load("Back.mp3")
		pygame.mixer.music.play()
	elif estado==1:	#ESTADO PREVIO A JUEGO (al que se llemaria desde el MAIN)
		sense.set_pixels(imgGo)
		time.sleep(1)
		sense.clear()
		seconds=int(time.time())+99
		timer=99
		estado=11
	elif estado==11:	#ESTADO DE JUEGO
		if (pygame.mixer.music.get_busy())==0:
			pygame.mixer.music.load("Back.mp3")
			pygame.mixer.music.play()

		for event in sense.stick.get_events():
			if event.action == "pressed":
				if event.direction == "middle":
					client.publish(TOPIC_boton, "1")

		sense.clear()

		if (disparo==1):
			disparo=0
			pygame.mixer.music.load("Laser_Gun.mp3")
			pygame.mixer.music.play()
			for i in range (0, 4):
				sense.set_pixels(imgDisparo[i])
				time.sleep(0.4)
				sense.clear()

		if atino==1:
			atino=0
			pygame.mixer.music.load("Explosion.mp3")
			pygame.mixer.music.play()
			print("Entro en atino")
			for i in range (0, 4):
				sense.set_pixels(imgAtino[i])
				time.sleep(0.15)
				sense.clear()
			if num>0:
				sense.show_letter(str(num), text_colour=msgC1)
				time.sleep(1)

		show_vidas(num, msgC1, msgC2)
		if timer==0:
			estado=4

	elif estado==2:		#ESTADO QUE NO SE PARA QUE ESTA
		pygame.mixer.stop
		sense.show_message("Fin Partida", text_colour=msgC1)
		estado=-1

	elif estado==3:		#ESTADO GANASTE
		pygame.mixer.stop
		sense.show_message("You WIN", text_colour=msgC1)
		estado==-1

	elif estado==4:		#ESTADO PERDISTE
		pygame.mixer.stop
		sense.show_message("You LOSE", text_colour=msgC1)
		estado==-1

	elif estado==0:		#ESTADO APAGAR
		pygame.mixer.stop
		sense.show_message("Chau!!!...", text_colour=msgC1)
		estado=-1

	if timer>0:			#IMPRESION DE TIEMPO
		timer=seconds-int(time.time())
		show_timer(timer,msgC1[0],msgC1[1],msgC1[2])
	time.sleep(.2)
