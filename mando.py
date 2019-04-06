import socket
import sys
import time
from sense_hat import SenseHat

host = '192.168.1.202' #'172.16.2.3' # 
sense = SenseHat()

port = 8080
size = 1024
x=0
y=0
j='W'

while True:
        y = int(sense.get_accelerometer_raw()['x']*50)
        x = int(sense.get_accelerometer_raw()['y']*50)

	for event in sense.stick.get_events():
		if event.action == "pressed":
			if event.direction == "middle":
				j='F'
			#elif event.direction == "up":
			#	y=-1
			#	x=0
			#elif event.direction == "down":
			#	y=+1
			#	x=0
			#elif event.direction == "left": 
			#	x=-1
			#	y=0
			#elif event.direction == "right":
			#	x=+1
			#	y=0


	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((host,port))
	data= ('mando' + ' ' + str(x) + ' ' + str(y) + ' ' + j)
	j='W'
	s.send(data)
	print('Se lanza envio de MANDO', data)
	time.sleep(.210)
	






