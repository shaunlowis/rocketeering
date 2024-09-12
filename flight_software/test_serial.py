import serial
import struct

# Configure the serial port
ser = serial.Serial('COM6', baudrate=115200, timeout=1)  # Adjust baudrate and timeout as needed

try:
    print("Listening on COM5...")
    while True:
        if ser.in_waiting > 0:
            # Read incoming character from the serial port
            incoming_bytes = ser.read(4)
            
            # Convert the 4 bytes to a float using IEEE 754 format
            float_val = struct.unpack('<f', incoming_bytes)[0]
            print(f"Received float: {float_val}")
except KeyboardInterrupt:
    print("Stopping...")
finally:
    ser.close()
