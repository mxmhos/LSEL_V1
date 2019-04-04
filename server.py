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
host = '192.168.1.202'
port = 8080
backlog = 5
size = 1024
remain = 1
mx='0'
my='0'
jb='W'

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
			if re.search('leds', data):
				# Update leds data
				data=('leds' + ' ' + mx + ' ' + my + ' ' + jb)
				mx='0'
				my='0'
				jb='W'
				print ('pedido leds', data)
				client.send(data)
			elif re.search('mando', data):
				# Update acc data
				lista = data.split(' ')
				mx=lista[1]
				my=lista[2]
				jb=lista[3]
				print ('info mando', data)
			elif re.search('clienteC', data):
				# Recibo cliente en C
				data=('clienteC' + ' ' + mx + ' ' + my + ' ' + jb)
				client.send(data)
				print ("cliente C: ", data, "  host:", host,"  port:", port)
#            else:
#                # Simply answer with a Not Found HTTP response
#                data = 'HTTP/1.1 404 Not Found\r\n'
#                client.send(data)
			client.close()
		elif s == sys.stdin:
			# Read keys and exit
			teclado = sys.stdin.readline()
			if teclado == "q":
				remain = 0
			else:
				print ('ingreso por teclado de: ', taclado)
server.close()
