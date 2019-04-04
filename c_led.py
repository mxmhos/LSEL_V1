import socket
import sys
import time
from sense_hat import SenseHat

host = '192.168.1.202'
sense = SenseHat()
X = [255, 0, 0] #Red
O = [255, 255, 255]
x=4
y=4
z=0
rz=0

port = 8080
size = 1024

while True:
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((host,port))
	s.send('leds')
	print('Se lanza pedido de info leds')
	data = s.recv(size)
	lista = data.split(' ')
	if lista[0]!='leds' and len(lista)!=4:
		print('Error en datos recibidos')
	else:
		rx=int(lista[1],10)
		ry=int(lista[2],10)
		rz=int(lista[3],10)
		print("rx: %d  ry: %d  rz: %d" % (rx, ry, rz))
		if y+ry<0:
			y=0
		elif y+ry>8:
			y=8
		else:
			y=y+ry
		if x+rx<0:
			x=0
		elif x+rx>8:
			x=8
		else:
			x=x+rx

		sense.clear()
		posx=x
		posy=y
		if posx>4:
		        posx=posx-1
		if posy>4:
		        posy=posy-1
		if x!=4 and y!=4:
		        sense.set_pixel(posx, posy, 255, 0, 0)
		else:
		        if x==4:
		                sense.set_pixel(3, posy, 255, 0, 0)
		                sense.set_pixel(4, posy, 255, 0, 0)
		        if y==4:
		                sense.set_pixel(posx, 3, 255, 0, 0)
		                sense.set_pixel(posx, 4, 255, 0, 0)
		        if y==4 and x==4:
		                sense.set_pixel(3, 3, 255, 0, 0)
	if rz>8 or rz <-8:
		break
        time.sleep(.1)



