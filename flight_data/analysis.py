import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV data
file_path = 'C:\\Users\\angus\\Documents\\Electronics\\Projects\\rocketeering\\flight_data\\LOG00037.csv'
data = pd.read_csv(file_path)

# Trim to only the flight section
start_index = 15128
end_index = -1
data = data[start_index:end_index].reset_index(drop=True)
data['timestamp'] = data['timestamp'] - data['timestamp'][0]
data['timestamp'] = data['timestamp'] / 1000

def basic_plot_from_df(data, x, y, title, xlabel, ylabel):
    plt.figure(figsize=(10, 6))
    plt.scatter(data[x], data[y])
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid()

# # Plot: GPS Altitude vs Time
# basic_plot_from_df(data, 'timestamp', 'gps_alt', 'GPS Altitude vs Time', 'Time (s)', 'Altitude (m)')
# # Plot: GPS Speed vs Time
# basic_plot_from_df(data, 'timestamp', 'gps_speed', 'GPS Speed vs Time', 'Time (s)', 'Speed (m/s)')


# # Plot: Latitude vs Longitude with color changing over time
# plt.figure(figsize=(10, 6))
# sc = plt.scatter(data['lati'], data['longi'], c=data['timestamp'], cmap='viridis', label='GPS Track')
# plt.xlabel('Latitude')
# plt.ylabel('Longitude')
# plt.title('GPS Track')
# plt.colorbar(sc, label='Time (s)')
# plt.legend()
# plt.grid()

# # Plot: Acceleration (x, y, z) vs Time on the same axis
# plt.figure(figsize=(10, 6))
# plt.plot(data['timestamp'], data['accel_x_g'], label='Accel X')
# plt.plot(data['timestamp'], data['accel_y_g'], label='Accel Y')
# plt.plot(data['timestamp'], data['accel_z_g'], label='Accel Z')
# plt.xlabel('Time (s)')
# plt.ylabel('Acceleration (g)')
# plt.title('Acceleration (X, Y, Z) vs Time')
# plt.legend()
# plt.grid()

# # Plot: Gyro (x, y, z) vs Time on the same axis
# plt.figure(figsize=(10, 6))
# plt.plot(data['timestamp'], data['gyro_x_dps'], label='Gyro X')
# plt.plot(data['timestamp'], data['gyro_y_dps'], label='Gyro Y')
# plt.plot(data['timestamp'], data['gyro_z_dps'], label='Gyro Z')
# plt.xlabel('Time (s)')
# plt.ylabel('Angular Velocity (dps)')
# plt.title('Gyro (X, Y, Z) vs Time')
# plt.legend()
# plt.grid()

# Plot: Thermocouple data vs Time
basic_plot_from_df(data, 'timestamp', 'tc_temp_C', 'Thermocouple vs Time', 'Time (s)', 'Temperature (C)')


# Convert pressure to altitude using the barometric formula
def pressure_to_altitude(pressure_pa):
    # Constants for the barometric formula
    P0 = 101440  # Measured ground level atmospheric pressure, in Pascals
    T0 = 25 + 273.15  # Measured sea level temperature on day of launch, in Kelvin
    g = 9.80665  # Earth-surface gravitational acceleration, in m/s^2
    L = 0.0065   # Temperature lapse rate, in K/m
    R = 8.31447  # Universal gas constant, in J/(mol·K)
    M = 0.0289644  # Molar mass of Earth's air, in kg/mol

    # Calculate altitude
    altitude_m = (T0 / L) * (1 - (pressure_pa / P0) ** (R * L / (g * M)))
    return altitude_m

data['calculated_altitude_m'] = pressure_to_altitude(data['pressure_pa'])

# Plot pressure and calculated altitude vs time with two y-axes
fig, ax1 = plt.subplots(figsize=(10, 6))

ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Pressure (Pa)', color='tab:blue')
ax1.plot(data['timestamp'], data['pressure_pa'], color='tab:blue', label='Pressure (Pa)')
ax1.tick_params(axis='y', labelcolor='tab:blue')

ax2 = ax1.twinx()
ax2.set_ylabel('Altitude (m)', color='tab:orange')
ax2.plot(data['timestamp'], data['calculated_altitude_m'], color='tab:orange', label='Calculated Altitude (m)')
ax2.tick_params(axis='y', labelcolor='tab:orange')

fig.tight_layout()
plt.title('Pressure and Calculated Altitude vs Time')
plt.grid()

plt.show()
