import serial
import time

timeStr = time.strftime("%Y_%m_%d_%H_%M")

arduinoPort = "COM3"
baud = 9600
fileName = timeStr + "_TestBenchData.csv"
# print(fileName)

ser = serial.Serial(arduinoPort, baud)

print("Connect to Arduino port: " + arduinoPort)

