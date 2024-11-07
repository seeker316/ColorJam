############### CLI VISUALIZER ######################

import serial
import time

ser = serial.Serial('/dev/ttyACM6', 9600,timeout=0.05)
raw_sensor = 0
sound_level = 0
counter = 0
sum = 0
try:
    while True:
        while counter < 250:
            if ser.in_waiting > 0:  
                try:
                    data = ser.readline().decode('utf-8').strip()
                    if len(data) == 1:
                        raw = ord(data)  #received data

                        counter = counter + 1

                        sum = sum + raw_sensor
                        reading_avg = sum / counter

                        sound_level = raw_sensor - reading_avg

                        bar_length = int((sound_level + 128) / 4) 
                        bar_length = max(0, min(bar_length, 64))  
                        
                        
                        print(f"[{'#' * bar_length}{' ' * (64 - bar_length)}] {sound_level}")
                except:
                    pass
        counter = 0
        sum = 0
        reading_avg = 0

except KeyboardInterrupt:
    print("Program interrupted by user.")
finally:
    ser.close()

