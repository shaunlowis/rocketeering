def find_sequence(sd_card_path, sequence_byte=0xFF, sequence_length=512):
    """
    Reads from the SD card byte by byte until it finds a sequence of 512 consecutive 0xA5 bytes.
    
    :param sd_card_path: Path to the SD card device.
    :param sequence_byte: Byte value to look for (default is 0xA5).
    :param sequence_length: Number of consecutive bytes to look for (default is 512).
    :return: None
    """
    sequence_count = 0
    address = 0
    
    try:
        # Open the SD card device in binary read mode
        with open(sd_card_path, 'rb') as sd_card:
            while True:
                byte = sd_card.read(1)
                
                # If no more data is read, stop searching
                if not byte:
                    print("End of file reached. Sequence not found.")
                    return
                
                # Convert byte to integer
                byte_value = ord(byte)
                
                if byte_value == sequence_byte:
                    sequence_count += 1
                    # Check if the sequence is long enough
                    if sequence_count == sequence_length:
                        print(f"Sequence of {sequence_length} bytes found starting at address {address - sequence_length + 1}.")
                        return
                else:
                    sequence_count = 0
                
                address += 1
                
    except FileNotFoundError:
        print(f"Error: The device path '{sd_card_path}' was not found.")
    except PermissionError:
        print("Error: You need elevated permissions to read from the SD card.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

# Example usage
find_sequence(r'\\.\D:')  # Replace with your actual SD card device path
