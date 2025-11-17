import time
data = '5a004d0ffb0ffb00000000000000000000000000000000fd04fd04fffafd04fd04fd04fd04fd04fd04fd04fd04fd04000000000000000000000000000000000000000000000000fd0400001c00'
data = bytearray.fromhex(data)
if (data[0] is not ord(b'\x5a')):
    exit()
f = open('log.csv','a')
f.write(time.strftime('%d/%m/%y %H:%M:%S')+',')
#sw sate
data = data[1::]
f.write(bin(int.from_bytes(data[0:2],byteorder='big')+4096)[3::] + ',')
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
    f.write(repr(int.from_bytes(data[0:2],byteorder='big')/16.0)[3::] + ',')
    data = data[2::]
for i in range(12):
    f.write(repr(int.from_bytes(data[0:2],byteorder='big')/16.0)[3::] + ',')
    data = data[2::]
f.write(repr(int.from_bytes(data[0:2],byteorder='big')/16.0)[3::] + ',')
data = data[2::]
f.write(repr(int.from_bytes(data[0:2],byteorder='big')/16.0)[3::] + ',')
data = data[2::]
f.write(repr(data[0]) + ',')
data = data[1::]
f.write(repr(data[0]) + ',')
data = data[1::]
f.write('\n')        
f.close()
