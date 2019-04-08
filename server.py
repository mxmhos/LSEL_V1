#!/usr/bin/env python
"""
Server
"""

import socket
import datetime
import select
import sys
import re

# variables
host =  '172.16.2.3' # '192.168.1.202' #
port = 8080
backlog = 5
size = 1024
remain = 1
mx='0'
my='0'
jb='W'
tf='N'
tf2='N'
topo_act='0'
topo_fire='0'
topo_next='0'
ident='0'


# Main function
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((host,port))
server.listen(backlog)

input_expected = [sys.stdin, server]

while remain:
	input_read, output_read, exception_read = select.select(input_expected, [], [])
	for s in input_read:
		if s == server:
			client, address = server.accept()
			data = client.recv(size)
#           if re.search('GET / HTTP/1.[0-1]', data):
#               data = """HTTP/1.1 200 OK\n\n""" + (open(web_addr)).read()
#		tabla= data[data.find("<table"):data.find("</table>")+8]
#		data = data.replace('Hab', 'No hay sensores conectados', 1)
#               print(data)
#		client.send(data)
			if re.search('mando', data):
				# Update acc data
				lista = data.split(' ')
				mx=lista[1]
				my=lista[2]
				jb=lista[3]
				print ('info mando', data)
				data=('leds' + ' ' + tf2)
				tf2='N'
				client.send(data)
			elif re.search('clienteC', data):
				# Recibo cliente en C
				data=('clienteC' + ' ' + mx + ' ' + my + ' ' + jb + ' ' + tf)
				jb='W'
				tf='N'
				client.send(data)
				#print ("cliente C: ", data, "  host:", host,"  port:", port)
			elif  re.search('topo',data):
				# Recibo dato topo0
				lista = data.split(' ')
				topo_act = lista[1]
				topo_fire = lista[2]
				print ('info topo:', data)
				if topo_act == 'off':
					if topo_next=='1':
						data=('topo on\n')
						print ('Mando instruccion encender topo')
						client.send(data)
					elif topo_next=='0':
						data=('topo off\n')
						client.send(data)
				elif topo_act== 'on':
					if topo_next=='0':
						data=('topo off\n')
						print ('Mando instruccion apagar topo')
						client.send(data)
					if topo_fire == 'si':
						tf='S'
						tf2='S'
						print ('Recibo topo fire')

#            else:
#                # Simply answer with a Not Found HTTP response
#                data = 'HTTP/1.1 404 Not Found\r\n'
#                client.send(data)
			client.close()
		elif s == sys.stdin:
			# Read keys and exit
			teclado = sys.stdin.readline()
			lista=teclado.split(' ')
			ident=lista[0]
			if ident == "topo":
				if lista[1]=="on":
					topo_next='1'
					print ('Cargo instruccion encender topo')
				elif lista[1]=="off":
					topo_next='0'
					print ('Cargo instruccion apagar topo')
				else:
					print ('Ident topo pero con error en orden')
			
			elif teclado.strip() == "o":
				topo_next='1'
				print ('Cargo instruccion encender topo')
			elif teclado.strip() == "n":
				topo_next='0'
				print ('Cargo instruccion apagar topo')


			elif teclado.strip() == "f":
				tf='S'
				tf2='S'
				print ('Cargo instruccion disparo efectivo')

			elif teclado.strip() == "1":
				mx='50'
				my='-50'
				jb='W'
				tf='N'
			elif teclado.strip() == "2":
				mx='0'
				my='-50'
				jb='W'
				tf='N'
			elif teclado.strip() == "3":
				mx='-50'
				my='-50'
				jb='W'
				tf='N'
			elif teclado.strip() == "4":
				mx='50'
				my='0'
				jb='W'
				tf='N'
			elif teclado.strip() == "5":
				mx='0'
				my='0'
				jb='F'
				tf='N'
			elif teclado.strip() == "6":
				mx='-50'
				my='0'
				jb='W'
				tf='N'
			elif teclado.strip() == "7":
				mx='50'
				my='50'
				jb='W'
				tf='N'
			elif teclado.strip() == "8":
				mx='0'
				my='50'
				jb='W'
				tf='N'
			elif teclado.strip() == "9":
				mx='-50'
				my='50'
				jb='W'
				tf='N'
			elif teclado.strip() == "q":
				remain = 0
server.close()
