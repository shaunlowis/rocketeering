import matplotlib.pyplot as plt

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

def plot_imu_data(data):
    # Create a 2x1 subplot for gyro and accel data
    fig2, (ax_accel, ax_gyro) = plt.subplots(2, 1, figsize=(10, 8), sharex=True, constrained_layout=True)

    # Plot accelerometer data
    ax_accel.plot(data["timestamp"], data["accel_x_g"], label="Accel X (g)")
    ax_accel.plot(data["timestamp"], data["accel_y_g"], label="Accel Y (g)")
    ax_accel.plot(data["timestamp"], data["accel_z_g"], label="Accel Z (g)")
    ax_accel.set_ylabel("Acceleration (g)")
    ax_accel.set_title("Accelerometer Data")
    ax_accel.legend()
    ax_accel.grid(True, linestyle="--", linewidth=0.5)

    # Plot gyroscope data
    ax_gyro.plot(data["timestamp"], data["gyro_x_dps"], label="Gyro X (°/s)")
    ax_gyro.plot(data["timestamp"], data["gyro_y_dps"], label="Gyro Y (°/s)")
    ax_gyro.plot(data["timestamp"], data["gyro_z_dps"], label="Gyro Z (°/s)")
    ax_gyro.set_xlabel("Time (s)")
    ax_gyro.set_ylabel("Angular Velocity (°/s)")
    ax_gyro.set_title("Gyroscope Data")
    ax_gyro.legend()
    ax_gyro.grid(True, linestyle="--", linewidth=0.5)

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

def animate_rocket_with_body_axes(df):
    """
    Animate rocket trajectory with a 3D rocket sprite and body frame axes.
    """
    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection='3d')

    # Trajectory line
    line, = ax.plot([], [], [], lw=2, c='blue')

    # Create a simple rocket shape (cone)
    def create_rocket_shape():
        base_size = 0.2
        height = 0.6
        vertices = np.array([
            [0, 0, height],  # tip
            [-base_size, -base_size, 0],
            [ base_size, -base_size, 0],
            [ base_size,  base_size, 0],
            [-base_size,  base_size, 0]
        ])
        faces = [
            [vertices[0], vertices[1], vertices[2]],
            [vertices[0], vertices[2], vertices[3]],
            [vertices[0], vertices[3], vertices[4]],
            [vertices[0], vertices[4], vertices[1]],
            [vertices[1], vertices[2], vertices[3], vertices[4]]  # base
        ]
        return vertices, faces

    rocket_vertices, rocket_faces = create_rocket_shape()
    rocket_poly = Poly3DCollection(rocket_faces, facecolor='red', edgecolor='k')
    ax.add_collection3d(rocket_poly)

    # Axes arrows for body frame
    axis_length = 0.5
    body_x = ax.quiver(0,0,0,0,0,0,length=axis_length,color='r',lw=2)
    body_y = ax.quiver(0,0,0,0,0,0,length=axis_length,color='g',lw=2)
    body_z = ax.quiver(0,0,0,0,0,0,length=axis_length,color='b',lw=2)

    # Set axes limits
    x_min, x_max = df['pos_x'].min(), df['pos_x'].max()
    y_min, y_max = df['pos_y'].min(), df['pos_y'].max()
    z_min, z_max = df['pos_z'].min(), df['pos_z'].max()
    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min, y_max)
    ax.set_zlim(z_min, z_max)
    ax.set_xlabel('X [m]')
    ax.set_ylabel('Y [m]')
    ax.set_zlabel('Z [m]')
    ax.set_title('Rocket Trajectory & Attitude')

    def rotate_and_translate(vertices, roll, pitch, yaw, pos):
        """Rotate vertices and translate to position"""
        R_roll = np.array([[1, 0, 0],
                           [0, np.cos(roll), -np.sin(roll)],
                           [0, np.sin(roll), np.cos(roll)]])
        R_pitch = np.array([[np.cos(pitch), 0, np.sin(pitch)],
                            [0, 1, 0],
                            [-np.sin(pitch), 0, np.cos(pitch)]])
        R_yaw = np.array([[np.cos(yaw), -np.sin(yaw), 0],
                          [np.sin(yaw), np.cos(yaw), 0],
                          [0, 0, 1]])
        R = R_yaw @ R_pitch @ R_roll
        return (R @ vertices.T).T + pos, R

    def update(i):
        # Update trajectory
        line.set_data(df['pos_x'][:i], df['pos_y'][:i])
        line.set_3d_properties(df['pos_z'][:i])

        # Get attitude
        roll = np.deg2rad(df['roll_deg'].iloc[i])
        pitch = np.deg2rad(df['pitch_deg'].iloc[i])
        yaw = np.deg2rad(df['yaw_deg'].iloc[i])
        pos = np.array([df['pos_x'].iloc[i],
                        df['pos_y'].iloc[i],
                        df['pos_z'].iloc[i]])

        # Rotate rocket vertices
        new_vertices, R = rotate_and_translate(rocket_vertices, roll, pitch, yaw, pos)

        # Update rocket faces
        for j, face in enumerate(rocket_faces):
            rocket_poly.set_verts([new_vertices[[0,1,2]] if j==0 else
                                   new_vertices[[0,2,3]] if j==1 else
                                   new_vertices[[0,3,4]] if j==2 else
                                   new_vertices[[0,4,1]] if j==3 else
                                   new_vertices[[1,2,3,4]]])

        # Update body axes
        body_axes = np.array([[axis_length,0,0],[0,axis_length,0],[0,0,axis_length]])
        axes_world = (R @ body_axes.T).T
        # Remove previous quivers and redraw
        for quiver in [body_x, body_y, body_z]:
            ax.collections.remove(quiver)
        body_x_new = ax.quiver(pos[0], pos[1], pos[2], axes_world[0,0], axes_world[0,1], axes_world[0,2], color='r', lw=2, length=axis_length)
        body_y_new = ax.quiver(pos[0], pos[1], pos[2], axes_world[1,0], axes_world[1,1], axes_world[1,2], color='g', lw=2, length=axis_length)
        body_z_new = ax.quiver(pos[0], pos[1], pos[2], axes_world[2,0], axes_world[2,1], axes_world[2,2], color='b', lw=2, length=axis_length)

        return line, rocket_poly, body_x_new, body_y_new, body_z_new

    ani = FuncAnimation(fig, update, frames=len(df), interval=50, blit=False)
    plt.show()
