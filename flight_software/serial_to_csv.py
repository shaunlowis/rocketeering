import serial
import threading
import csv
from collections import deque
import time
import msvcrt  # For capturing keyboard input on Windows

# Configuration for serial port and circular buffer
SERIAL_PORT = 'COM6'
BAUDRATE = 115200
BUFFER_SIZE = 10  # Size of circular buffer

# Circular buffer (deque)
circ_buffer = deque(maxlen=BUFFER_SIZE)

# Condition variable to synchronize threads
buffer_not_empty = threading.Condition()

# Flag to signal threads to stop
stop_threads = threading.Event()

# Thread function to read lines from serial port
def read_serial():
    ser = serial.Serial(SERIAL_PORT, baudrate=BAUDRATE, timeout=1)
    print(f"Listening on {SERIAL_PORT}...")
    try:
        while not stop_threads.is_set():
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').strip()  # Read a line from the serial port
                if line:
                    with buffer_not_empty:
                        circ_buffer.append(line)  # Add the line to the circular buffer
                        print(f"Added to buffer: {line}")
                        buffer_not_empty.notify()  # Notify the other thread that the buffer is not empty
    except Exception as e:
        print(f"Error: {e}")
    finally:
        ser.close()

# Thread function to write lines to CSV
def write_to_csv():
    with open('output.csv', mode='a', newline='') as csvfile:
        csv_writer = csv.writer(csvfile)
        while not stop_threads.is_set():
            with buffer_not_empty:
                while len(circ_buffer) == 0:
                    buffer_not_empty.wait()  # Wait until the buffer is not empty
                    if stop_threads.is_set():  # Exit if stop signal is set
                        break
                if len(circ_buffer) > 0:
                    line = circ_buffer.popleft()  # Get the line from the buffer
                    print(f"Writing to CSV: {line}")
                    csv_writer.writerow([line])  # Write the line to the CSV

# Function to detect Esc key press
def wait_for_esc():
    print("Press 'Esc' to stop...")
    while True:
        if msvcrt.kbhit():
            if ord(msvcrt.getch()) == 27:  # 27 is the ASCII code for Esc
                print("Esc pressed, stopping...")
                stop_threads.set()  # Signal threads to stop
                break
        time.sleep(0.1)

# Main function to start threads
def main():
    # Create and start the serial reader and CSV writer threads
    reader_thread = threading.Thread(target=read_serial)
    writer_thread = threading.Thread(target=write_to_csv)
    esc_thread = threading.Thread(target=wait_for_esc)

    reader_thread.start()
    writer_thread.start()
    esc_thread.start()

    reader_thread.join()
    writer_thread.join()
    esc_thread.join()

if __name__ == "__main__":
    main()
