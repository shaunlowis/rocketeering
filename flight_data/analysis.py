import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

import matplotlib as mpl
from matplotlib import font_manager

# Some prettification
plt.rcParams["figure.dpi"] = 250
mpl.rc("axes", labelsize=10, titlesize=16, linewidth=0.2)
mpl.rc("legend", fontsize=10)
mpl.rc("xtick", labelsize=12)
mpl.rc("xtick.major", size=2, width=0.5)
mpl.rc("xtick.minor", size=1, width=0.25, visible=True)
mpl.rc("ytick", labelsize=12)
mpl.rc("ytick.major", size=2, width=0.5)
mpl.rc("ytick.minor", size=1, width=0.25, visible=True)

# Font
font_path = os.path.join(os.getcwd(), "img", "AnonymousPro-Regular.ttf")
font_manager.fontManager.addfont(font_path)
prop = font_manager.FontProperties(fname=font_path)

mpl.rcParams["font.family"] = prop.get_name()  # Use the font's name

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


def plot_altitude(data):
    # Plot pressure and calculated altitude vs time with two y-axes
    fig, ax1 = plt.subplots(figsize=(10, 6), constrained_layout=True)

    ax1.set_xlabel("Time (s)")
    ax1.set_ylabel("Pressure (Pa)", color="tab:blue")
    ax1.plot(
        data["timestamp"], data["pressure_pa"], color="tab:blue", label="Pressure (Pa)"
    )
    ax1.tick_params(axis="y", labelcolor="tab:blue")

    ax2 = ax1.twinx()
    ax2.set_ylabel("Altitude (m)", color="tab:orange")
    ax2.plot(
        data["timestamp"],
        data["calculated_altitude_m"],
        color="tab:orange",
        label="Calculated Altitude (m)",
    )
    ax2.tick_params(axis="y", labelcolor="tab:orange")

    ax1.set_title("Pressure and Calculated Altitude vs Time")
    ax2.grid(which="major", linestyle="--", linewidth=0.5)

    plt.savefig("altitude.png")


def plot_thermocouple(data):
    fig, ax1 = plt.subplots(figsize=(10, 6), constrained_layout=True)

    ax1.set_xlabel("Time (s)")
    ax1.set_ylabel("Temperature [C]", color="red")
    ax1.plot(data["timestamp"], data["tc_temp_C"], color="red")
    ax1.tick_params(axis="y", labelcolor="red")

    ax1.set_title("Motor exhaust temperature vs Time")
    ax1.grid(which="major", linestyle="--", linewidth=0.5)

    plt.savefig("thermocouple.png")


def plot_IMU(data):
    fig = plt.figure(figsize=(8.5, 6), constrained_layout=True)
    ax = fig.add_subplot(projection="3d")  # 3D scatter plot

    # IMU acceleration data
    x = data["accel_x_g"]
    y = data["accel_y_g"]
    z = data["accel_z_g"]

    # Color by time if available, else default color
    c = data.get(
        "time", np.linspace(0, 1, len(data["timestamp"]))
    )  # Use time if available

    scatter = ax.scatter(x, y, z, c=c, cmap="viridis", edgecolors="k", alpha=0.8)

    ax.set_xlabel("X-axis (G)")
    ax.set_ylabel("Y-axis (G)")
    ax.set_zlabel("Z-axis (G)")
    ax.set_title("3D Scatter Plot of IMU Data")

    # Adjust colorbar position
    cbar_ax = fig.add_axes([0.9, 0.2, 0.03, 0.6])  # [left, bottom, width, height]
    cbar = fig.colorbar(scatter, cax=cbar_ax)
    cbar.set_label("Normalized time")

    plt.savefig("imu_data.png")


def plot_battery_stats(data):
    # Plot pressure and calculated altitude vs time with two y-axes
    fig, ax1 = plt.subplots(figsize=(10, 6), constrained_layout=True)

    ax1.set_xlabel("Time (s)")
    ax1.set_ylim(
        [
            0,
            100,
        ]
    )

    ax1.set_ylabel("Current [mA]", color="tab:orange")
    ax1.plot(
        data["timestamp"],
        data["batt_current_mA"],
        color="tab:orange",
    )
    ax1.tick_params(axis="y", labelcolor="tab:orange")

    ax2 = ax1.twinx()
    ax2.set_ylabel("Voltage [V]", color="tab:blue")
    ax2.plot(data["timestamp"], data["batt_voltage_V"], color="tab:blue")
    ax2.tick_params(axis="y", labelcolor="tab:blue")
    ax2.set_ylim(
        [
            data["batt_voltage_V"].min() - 0.05 * data["batt_voltage_V"].min(),
            data["batt_voltage_V"].max() + 0.05 * data["batt_voltage_V"].max(),
        ]
    )

    ax1.set_title("Battery use vs time")
    ax2.grid(which="major", linestyle="--", linewidth=0.5)

    plt.savefig("battery.png")


def main():
    # Load the CSV data
    file_path = os.path.join(os.getcwd(), "flight_data", "LOG00037.csv")
    data = pd.read_csv(file_path)

    # Trim to only the flight section
    start_index = 15128
    end_index = -1
    data = data[start_index:end_index].reset_index(drop=True)
    data["timestamp"] = data["timestamp"] - data["timestamp"][0]
    data["timestamp"] = data["timestamp"] / 1000

    data["calculated_altitude_m"] = pressure_to_altitude(data["pressure_pa"])

    
    plot_altitude(data)
    # plot_thermocouple(data)
    plot_IMU(data)
    # plot_battery_stats(data)

    # for col in data.columns:
    #     print(col)


if __name__ == main():
    main()
