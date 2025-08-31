import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
from plotting import *

# Convert pressure to altitude using the barometric formula
def pressure_to_altitude(pressure_pa):
    # Constants for the barometric formula
    P0 = 101440  # Measured ground level atmospheric pressure, in Pascals
    T0 = 25 + 273.15  # Measured sea level temperature on day of launch, in Kelvin
    g = 9.80665  # Earth-surface gravitational acceleration, in m/s^2
    L = 0.0065  # Temperature lapse rate, in K/m
    R = 8.31447  # Universal gas constant, in J/(mol·K)
    M = 0.0289644  # Molar mass of Earth's air, in kg/mol

    # Calculate altitude
    altitude_m = (T0 / L) * (1 - (pressure_pa / P0) ** (R * L / (g * M)))
    return altitude_m

def calculate_average_accelerometer_data_on_rail(data):
    # Calculate the average accelerometer data on the rail
    average_accel = data[["accel_x_g", "accel_y_g", "accel_z_g"]].mean()
    return average_accel

def main():
    # Load the CSV data
    file_path = "LOG00037.csv"
    data = pd.read_csv(file_path)

    # Trim data to desired sections
    flight_start_index = 12128
    flight_end_index = -1
    flight_data = data[flight_start_index:flight_end_index].reset_index(drop=True)
    flight_data["timestamp"] = flight_data["timestamp"] - flight_data["timestamp"][0]
    flight_data["timestamp"] = flight_data["timestamp"] / 1000

    rail_start_index = 11000
    rail_end_index = 15200
    rail_data = data[rail_start_index:rail_end_index].reset_index(drop=True)
    rail_data["timestamp"] = rail_data["timestamp"] - rail_data["timestamp"][0]
    rail_data["timestamp"] = rail_data["timestamp"] / 1000

    # Calculate altitude from baro data
    flight_data["baro_calculated_altitude_m"] = pressure_to_altitude(flight_data["pressure_pa"])

    flight_data.to_csv("flight_data_trimmed.csv", index=False)
    plot_imu_data(flight_data)

    # Calculate 
    #plot_altitude(flight_data)
    average_accel = calculate_average_accelerometer_data_on_rail(rail_data)
    print("Average Accelerometer Data on Rail (g):")
    print(average_accel)
    total_accel_magnitude = np.linalg.norm(average_accel)
    print(f"Total Acceleration Magnitude on Rail (g): {total_accel_magnitude:.4f}")
    # Determine angle of launch rail in NED frame
    # Average acceleration vector on rail (in g)
    accel_vector = average_accel.values
    # Normalize to get direction
    accel_dir = accel_vector / np.linalg.norm(accel_vector)
    # In NED frame, gravity points down (positive Z), so launch rail angle from vertical:
    # Angle between accel_dir and [0, 0, 1] (down)
    ned_down = np.array([0, 0, 1])
    cos_theta = np.dot(accel_dir, ned_down)
    launch_angle_deg = np.degrees(np.arccos(cos_theta))
    print(f"Launch Rail Angle from Vertical (deg, NED frame): {launch_angle_deg:.2f}")


    plt.show()

if __name__ == "__main__":
    main()