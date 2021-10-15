#!/usr/bin/env python3

"""

  Copyright (C) 2015  University of Alberta
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
 
  @file dfgm_sim.py
  @author Andrew Williams
  @date 2020-8-10

"""

import serial
import struct
import time

############################# Initialization #############################
try:
    # Open Serial Port
    serialPort = serial.Serial(port = "/dev/ttyUSB0",
                            baudrate=9600,
                            bytesize=8,
                            timeout=0.5,
                            stopbits=serial.STOPBITS_TWO)
except:
    print("Error opening serial port.")
    exit(1)

# confirm connection
print("Connected to " + serialPort.name + " at " + str(serialPort.baudrate) + " baud...")
# hold data coming over UART
serialString = "" 
# Open DFGM data files
dfgm_file1 = open("0c4R0196.bin", "rb")
# dfgm_file2 = open("mag_data.txt", "r")
# dfgm_file3 = open("output.txt", "r")

############################# RAW -> MAG CONVERSION REQUEST #############################
print("Opened file " + dfgm_file1.name)
while(1):
    # Write one line of data over UART
    line = dfgm_file1.read(1248)
    serialPort.write(line)

    # Read data out of the buffer
    serialString = serialPort.read(1248)

    # compare sent vs received data
    i = 0
    while (i < 100):
        print(str(struct.unpack('>hh', line[(36+(12*i)):(40+(12*i))])) + "\t" +
              str(struct.unpack('>hh', line[(40+(12*i)):(44+(12*i))])) + "\t" + 
              str(struct.unpack('>hh', line[(44+(12*i)):(48+(12*i))])) + "\t ->   ", end=' ')
        print("("+str(round(struct.unpack('>f', serialString[(36+(12*i)):(40+(12*i))])[0], 2))+", "+
                  str(round(struct.unpack('>f', serialString[(40+(12*i)):(44+(12*i))])[0], 2))+", "+
                  str(round(struct.unpack('>f', serialString[(44+(12*i)):(48+(12*i))])[0], 2))+")")
        i+=1
    time.sleep(1);
############################# CREATE MAG DATA FILE #############################
# # Open DFGM data file
# dfgm_file = open("0c4R0196.bin", "rb")
# print("Opened file " + dfgm_file.name)
# f = open("mag_data.txt", "a")
# while(1):
#     line = dfgm_file.read(1248)
#     if not line:
#         break
#     serialPort.write(line)
#     serialString = serialPort.read(1248)
#     i = 0
#     while (i < 100):
#         f.write('0 ' + str(round(struct.unpack('>f', serialString[(36+(12*i)):(40+(12*i))])[0], 2)) +
#                 ' '  + str(round(struct.unpack('>f', serialString[(40+(12*i)):(44+(12*i))])[0], 2)) +
#                 ' '  + str(round(struct.unpack('>f', serialString[(44+(12*i)):(48+(12*i))])[0], 2)) + ' 0\n')
#         i += 1
# f.close()    


############################# 100 TO 1 FILTER REQUEST #############################
# scanfCount = 5
# maxlen = 33
# while(1):
#     if(serialPort.in_waiting > 0):
#         serialString = serialPort.read(1)
#         print(serialString.decode('Ascii'), end="")
#         if serialString.decode('Ascii') == 's' and scanfCount >= 0:
#             scanfCount = 1
#         elif serialString.decode('Ascii') == 'c' and scanfCount >= 1:
#             scanfCount = 2
#         elif serialString.decode('Ascii') == 'a' and scanfCount >= 2:
#             scanfCount = 3
#         elif serialString.decode('Ascii') == 'n' and scanfCount >= 3:
#             scanfCount = 4
#         elif serialString.decode('Ascii') == 'f' and scanfCount >= 4:
#             print("")
#             scanf received: output 100 lines
#         i = 0
#         while (i < 100):
#             line = dfgm_file2.readline()
#             if not line:
#                 break
#             cols = line.split(" ")
#             s = struct.pack('>ddd', float(cols[1]), float(cols[2]), float(cols[3]))
#             # print(struct.unpack('>ddd', s))
#             # print(len(s))
#             # print(len(s))
#             # line = line.rstrip('\n').ljust(33, '0')
#             # print(line, end="")
#             serialPort.write(s)
#             i+=1
#         else:
#             count = 0