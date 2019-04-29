import socket
import sys
import time
from sense_hat import SenseHat

host = '172.16.2.3' # '192.168.1.202' # 
sense = SenseHat()

port = 8080
size = 1024

R = [255, 0, 0] #Red
W = [255, 255, 255] # White
N = [0, 0, 0] #none

img1= [[R, R, R, N, N, R, R, R, R, N, N, N, N, N, N, R, R, N, N, N, N, N, N, R, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, R, N, N, N, N, N, N, R, R, N, N, N, N, N, N, R, R, R, R, R, R, R, R, R], [ N, N, N, N, N, N, N, N, N, R, R, R, R, R, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, R, R, R, R, R, N, N, N, N, N, N, N, N, N], [N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, R, R, R, R, N, N, N, N, R, N, N, R, N, N, N, N, R, N, N, R, N, N, N, N, R, R, R, R, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N], [ N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, R, R, N, N, N, N, N, N, R, R, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N]]

img2= [[
N, N, N, N, N, N, N, N, 
N, N, N, N, N, N, N, N, 
N, N, N, W, R, N, N, N, 
N, N, W, R, W, R, N, N, 
N, N, R, W, R, W, N, N, 
N, N, N, R, W, N, N, N, 
N, N, N, N, N, N, N, N, 
N, N, N, N, N, N, N, N], [

N, N, N, N, N, N, N, N, 
N, N, R, W, R, W, N, N, 
N, R, W, R, W, R, W, N, 
N, W, R, W, R, W, R, N, 
N, R, W, R, W, R, W, N, 
N, W, R, W, R, W, R, N, 
N, N, W, R, W, R, N, N, 
N, N, N, N, N, N, N, N], [

N, N, R, W, R, W, N, N, 
N, R, W, R, W, R, W, N, 
R, W, R, W, R, W, R, W, 
W, R, W, R, W, R, W, R, 
R, W, R, W, R, W, R, W, 
W, R, W, R, W, R, W, R, 
N, W, R, W, R, W, R, N, 
N, N, W, R, W, R, N, N], [

N, N, W, R, W, R, N, N, 
N, W, R, W, R, W, R, N, 
W, R, W, R, W, R, W, W, 
R, W, R, W, R, W, R, R, 
W, R, W, R, W, R, W, W, 
R, W, R, W, R, W, R, R, 
N, R, W, R, W, R, W, N, 
N, N, R, W, R, W, N, N]]

x=0
y=0
j='W'
t='N'
count = 9
sense.rotation = 270

while True:
        y = int(sense.get_accelerometer_raw()['x']*50)
        x = int(sense.get_accelerometer_raw()['y']*100)

	for event in sense.stick.get_events():
		if event.action == "pressed":
			if event.direction == "middle":
				j='F'
				if count==-2:
					count = 9

	if count==0:
		j="fin"

	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((host,port))
	data= ('mando' + ' ' + str(x) + ' ' + str(y) + ' ' + j)
	s.send(data)
	#print('Se lanza envio de MANDO: ', data)

	if count==0:
		count=-1
		j='N'
	
	data = s.recv(size)
	#print('Recibo info dede SERVER: ', data)
	
	if count==-1:
		sense.show_message("You WIN", text_colour=[255, 0, 0])
		time.sleep(3)
		count=-2

	sense.clear()	
	if j=='F':
		j='W'
		for i in range (0, 4):
			sense.set_pixels(img1[i])
			time.sleep(0.4)
			sense.clear()
	
	lista = data.split(' ')
	if lista[0]!='leds' and len(lista)!=2:
		print('Error en datos recibidos')
	else:
		t = lista [1]
		if t == "S":
			for i in range (0, 4):
				sense.set_pixels(img2[i])
				time.sleep(0.15)
				sense.clear()
			if count>0:
				count = count -1
		elif count>0:
			sense.show_letter(str(count), text_colour=[255, 0, 0])
		
	time.sleep(.390)
	






