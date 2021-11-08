import serial
import time

dirPath = ".data/"
timeStr = time.strftime("%Y_%m_%d_%H_%M")

arduinoPort = "COM3"
baud = 9600
fileName = dirPath + timeStr + "_TestBenchData.csv"
# print(fileName)

#ser = serial.Serial(arduinoPort, baud)

print("Connect to Arduino port: " + arduinoPort)

file = open(fileName, "a")
print("Created file")

file.close()