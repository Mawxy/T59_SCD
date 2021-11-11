import serial
import time

dirPath = ".data/"
timeStr = time.strftime("%Y_%m_%d_%H_%M")

arduinoPort = "COM3"
baud = 9600
fileName = dirPath + timeStr + "_TestBenchData.csv"
# print(fileName)

samples = 121
line = 0

ser = serial.Serial(arduinoPort, baud)

print("Connect to Arduino port: " + arduinoPort)

file = open(fileName, "a")
print("Created file")

while line <= samples:
    getData = str(ser.readline())
    data = getData[2:][:-5]
    print(data)

    file.write(data)
    file.write('\n')

    line = line + 1

print("Collection done")

ser.close()
file.close()
