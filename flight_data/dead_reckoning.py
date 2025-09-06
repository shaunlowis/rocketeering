"""
reconstruct_flight_timestamped.py

Dead-reckoning reconstruction (IMU-only) for your rocket flight.

Inputs: CSV file with columns
  timestamp, accel_x_g, accel_y_g, accel_z_g, gyro_x_dps, gyro_y_dps, gyro_z_dps

Assumptions:
  - timestamp is in seconds (float).
  - Data is ~20 Hz but not perfectly uniform; we use actual deltas between rows.

Outputs:
  - printed initial attitude (roll, pitch, launch rail angle)
  - plots of Euler angles and trajectory
  - CSV "reconstruction_out.csv" with time, Euler angles, position, velocity
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from math import sin, cos, atan2, asin, sqrt, degrees
from plotting import *
from scipy.spatial.transform import Rotation as sciR

# -------------------------
# User settings
# -------------------------
FNAME = "flight_data_trimmed.csv"      # replace with your filename
OUT_CSV = "reconstruction_out.csv"
g_val = 9.80665                # m/s^2
deg2rad = np.pi / 180.0
rad2deg = 180.0 / np.pi
gravity_vector_in_world_frame = np.array([0, 0, -g_val])
flight_start_index = 3190


# Axis mapping: IMU Y axis pointed along rocket nose (tail->nose = body X).
# So: body_x <= imu_y, body_y <= imu_z, body_z <= imu_x (right-handed).
axis_map = {'x': 'y', 'y': 'z', 'z': 'x'}
axis_sign = {'x': 1, 'y': 1, 'z': 1}

# -------------------------
# Quaternion helpers using scipy (x, y, z, w order)
# -------------------------

def quat_mult(q, r):
    # q, r: [x, y, z, w]
    return (sciR.from_quat(q) * sciR.from_quat(r)).as_quat()

def quat_normalize(q):
    return sciR.from_quat(q).as_quat() / np.linalg.norm(q)

def quat_propagate(q, omega_body, dt):
    """Propagate quaternion with body angular rate omega (rad/s)."""
    # omega_body: [x, y, z], q: [x, y, z, w]
    rot = sciR.from_quat(q)
    # Small-angle approximation for delta rotation
    delta_rot = sciR.from_rotvec(omega_body * dt)
    q_new = (rot * delta_rot).as_quat()
    return q_new / np.linalg.norm(q_new)


# -------------------------
# Load data
# -------------------------
df = pd.read_csv(FNAME)

time = df['timestamp'].to_numpy(dtype=float)
dt_array = np.zeros(len(time))
dt_array[1:] = time[1:] - time[:-1]
dt_array[0] = dt_array[1]

accel_imu = df[['accel_x_g','accel_y_g','accel_z_g']].to_numpy(dtype=float)
gyro_imu  = df[['gyro_x_dps','gyro_y_dps','gyro_z_dps']].to_numpy(dtype=float)


# Map IMU axes to body frame
N = len(df)
accel_body = np.zeros((N,3))
gyro_body  = np.zeros((N,3))
for i in range(N):
    imu_a = {'x': accel_imu[i,0], 'y': accel_imu[i,1], 'z': accel_imu[i,2]}
    imu_g = {'x': gyro_imu[i,0],  'y': gyro_imu[i,1],  'z': gyro_imu[i,2]}
    accel_body[i] = np.array([
        axis_sign['x']*imu_a[axis_map['x']],
        axis_sign['y']*imu_a[axis_map['y']],
        axis_sign['z']*imu_a[axis_map['z']]
    ])
    gyro_body[i] = np.array([
        axis_sign['x']*imu_g[axis_map['x']],
        axis_sign['y']*imu_g[axis_map['y']],
        axis_sign['z']*imu_g[axis_map['z']]
    ])

# Convert to SI
accel_body *= g_val          # g -> m/s^2
gyro_body  *= deg2rad        # dps -> rad/s

# -------------------------
# Initial attitude from static launch rail
# -------------------------
stationary_mask = np.arange(len(time)) < flight_start_index
accel_mean = accel_body[stationary_mask].mean(axis=0)
gyro_mean  = gyro_body[stationary_mask].mean(axis=0)

# Roll/pitch from accel (yaw unobservable)
ax, ay, az = accel_mean / g_val
roll0  = atan2(ay, az)
pitch0 = atan2(-ax, np.sqrt(ay*ay + az*az))
yaw0   = 0.0
q = sciR.from_euler('xyz', [roll0, pitch0, yaw0]).as_quat()  # scipy uses [x, y, z, w] order

# "Launch rail angle": angle between body X axis and vertical
up_body = accel_mean / np.linalg.norm(accel_mean)
launch_angle = degrees(np.arccos(np.clip(up_body[0], -1, 1)))

print(f"Initial roll={degrees(roll0):.2f}°, pitch={degrees(pitch0):.2f}°, yaw=0° (set)")
print(f"Launch rail angle from vertical: {launch_angle:.2f}°")

# Estimate constant biases
R0 = sciR.from_quat(q).as_matrix()  # scipy uses [x, y, z, w] order
expected_f = -R0.T @ gravity_vector_in_world_frame # expected accel in world frame when stationary. Z down/up?
accel_bias = accel_mean - expected_f
gyro_bias  = gyro_mean

print("Accel bias (m/s^2):", accel_bias)
print("Gyro bias (rad/s):", gyro_bias)

# -------------------------
# Integration loop
# -------------------------
pos = np.zeros((N,3))
vel = np.zeros((N,3))
euler = np.zeros((N,3))
quat = np.zeros((N,4))

quat[0] = q
euler[0] = [roll0, pitch0, yaw0]


# Subtract acceleration bias and convert into world frame by quaternion rotation.
a_imu_world_prev = R0 @ (accel_body[0]-accel_bias)
# Get true acceleration by adding gravity vector (remove gravity from the acceleration)
a_world_prev =  a_imu_world_prev + gravity_vector_in_world_frame

for k in range(flight_start_index, N-1):
    dt = dt_array[k+1]
    # attitude update
    omega = gyro_body[k] - gyro_bias
    q = quat_propagate(q, omega, dt)
    quat[k+1] = q
    rot = sciR.from_quat(q)  # scipy uses [x, y, z, w] order
    R = rot.as_matrix()

    # euler angles (ZYX)   
    roll, pitch, yaw = rot.as_euler('xyz', degrees=True)
    euler[k+1] = [roll, pitch, yaw]

    # accel → world with g removed
    f_b = accel_body[k+1] - accel_bias
    a_world = R @ f_b + gravity_vector_in_world_frame

    # integrate v,p with trapezoidal
    v_new = vel[k] + 0.5*(a_world_prev + a_world)*dt
    p_new = pos[k] + 0.5*(vel[k] + v_new)*dt
    vel[k+1] = v_new
    pos[k+1] = p_new

    a_world_prev = a_world

# -------------------------
# Add results to df
# -------------------------
df['timestamp'] = time
df['pos_x'] = pos[:,0]
df['pos_y'] = pos[:,1]
df['pos_z'] = pos[:,2]
df['vel_x'] = vel[:,0]
df['vel_y'] = vel[:,1]
df['vel_z'] = vel[:,2]
df['roll_deg'] = euler[:,0]
df['pitch_deg'] = euler[:,1]
df['yaw_deg'] = euler[:,2]

# Keep only rows where the index > flight_start_index
df = df[df.index > flight_start_index].copy()

# Reset index
df.reset_index(drop=True, inplace=True)

# Shift timestamps so the first timestamp is 0
df['timestamp'] -= df['timestamp'].iloc[0]


df.to_csv(OUT_CSV, index=False)
print("Wrote", OUT_CSV)


# -------------------------
# Plots
# -------------------------
plt.figure(figsize=(10,6))
plt.subplot(3,1,1)
plt.plot(df['timestamp'], df['roll_deg'], label='roll')
plt.plot(df['timestamp'], df['pitch_deg'], label='pitch')
plt.plot(df['timestamp'], df['yaw_deg'], label='yaw')
plt.ylabel('deg'); plt.legend(); plt.title('Attitude')

plt.subplot(3,1,2)
plt.plot(df['timestamp'], df['pos_x'], label='X')
plt.plot(df['timestamp'], df['pos_y'], label='Y')
plt.plot(df['timestamp'], df['pos_z'], label='Z')
plt.plot(df['timestamp'], df['baro_calculated_altitude_m'], label='Baro Altitude', linestyle='--', color='gray')
plt.ylabel('m'); plt.xlabel('time (s)'); plt.legend(); plt.title('Position')

plt.subplot(3,1,3)
plt.plot(df['timestamp'], df['vel_x'], label='X')
plt.plot(df['timestamp'], df['vel_y'], label='Y')
plt.plot(df['timestamp'], df['vel_z'], label='Z')
plt.ylabel('m/s'); plt.xlabel('time (s)'); plt.legend(); plt.title('Velocity')

plt.tight_layout(); plt.show()