import socket
import sys
import time
from sense_hat import SenseHat

host = '192.168.1.202'
sense = SenseHat()

port = 8080
size = 1024
x=0
y=0
z=0

while True:
        x = int(sense.get_accelerometer_raw()['x']*8)
        y = int(sense.get_accelerometer_raw()['y']*8)
	z = int(sense.get_accelerometer_raw()['y']*10)

	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((host,port))
	data= ('acc' + ' ' + str(x) + ' ' + str(y) + ' ' + str(z))
	s.send(data)
	print('Se lanza envio de ACC', data)
	time.sleep(.4)
	
	if z>8 or z<-8:
		break






