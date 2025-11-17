import socket
import socketserver
import csv
import time
import os

packet_counter = 0

class MyUDPHandler(socketserver.BaseRequestHandler):
    """
    The request handler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """
    
    def handle(self):
        global packet_counter
        data = self.request[0]
        print(data)
        if (data[0] is not ord(b'\x5a')):
            return
        f1 = open('lograw.csv','a')
        f1.write(time.strftime('%d/%m/%y %H:%M:%S')+',')
        f1.write(data.hex()+'\n')
        f1.close()
        f = open('log.csv','a')
        f.write(time.strftime('%d/%m/%y %H:%M:%S')+',')
        #sw sate
        data = data[3::]
        f.write('"'+bin(int.from_bytes(data[0:2],byteorder='big')+4096)[3::]+'"' + ',')
        data = data[2::]
        f.write(bin(int.from_bytes(data[0:2],byteorder='big')+4096)[3::] + ',')
        data = data[2::]
        for byte in data[0:12]:
            f.write(repr(byte)+ ',')
        data = data[12::]
        f.write(bin(int.from_bytes(data[0:2],byteorder='big')+4096)[3::] + ',')
        data = data[2::]
        f.write(bin(int.from_bytes(data[0:2],byteorder='big')+4096)[3::] + ',')
        data = data[2::]
        for i in range(12):
            f.write(repr((int.from_bytes(data[0:2],byteorder='big')-65536)/16.0) + ',')
            data = data[2::]
        for i in range(12):
            f.write(repr((int.from_bytes(data[0:2],byteorder='big')-65536)/16.0) + ',')
            data = data[2::]
        f.write(repr((int.from_bytes(data[0:2],byteorder='big')-65536)/16.0) + ',')
        data = data[2::]
        f.write(repr((int.from_bytes(data[0:2],byteorder='big')-65536)/16.0) + ',')
        data = data[2::]
        f.write(repr(data[0]) + ',')
        data = data[1::]
        f.write(repr(data[0]) + ',')
        data = data[1::]
        f.write('\n')        
        f.close()
        packet_counter = packet_counter+1
        os.system('cls')
        print(packet_counter)



HOST, PORT = '', 5010
server = socketserver.UDPServer((HOST, PORT), MyUDPHandler)
server.allow_reuse_address = True
server.serve_forever()
