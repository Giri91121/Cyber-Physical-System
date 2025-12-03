import serial
import keyboard # Library untuk menekan tombol keyboard secara virtual
import time

# Ganti 'COM3' dengan port Arduino Agi (Cek di Device Manager)
arduino = serial.Serial('COM3', 9600) 
time.sleep(2) # Tunggu koneksi stabil

print("Bridge Aktif! Gerakkan Analog...")

while True:
    if arduino.in_waiting:
        # Baca data dari Arduino
        data = arduino.readline().decode('utf-8').strip()
        
        # Tekan tombol keyboard sesuai data
        if data == "LEFT":
            keyboard.press('left')
            time.sleep(0.03)
            keyboard.release('left')
        elif data == "RIGHT":
            keyboard.press('right')
            time.sleep(0.03)
            keyboard.release('right')
        elif data == "UP":
            keyboard.press('up')
            time.sleep(0.03)
            keyboard.release('up')
        elif data == "DOWN":
            keyboard.press('down')
            time.sleep(0.03)
            keyboard.release('down')