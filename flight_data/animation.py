import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from scipy.spatial.transform import Rotation as R

# Load data
df = pd.read_csv("reconstruction_out.csv")

# Extract values
t = df["timestamp"].values
roll = np.deg2rad(df["roll_deg"].values)
pitch = np.deg2rad(df["pitch_deg"].values)
yaw = np.deg2rad(df["yaw_deg"].values)
pos = df[["pos_x", "pos_y", "pos_z"]].values

def Rx(phi):
    c,s = np.cos(phi), np.sin(phi)
    return np.array([[1,0,0],[0,c,-s],[0,s,c]])

def Ry(theta):
    c,s = np.cos(theta), np.sin(theta)
    return np.array([[c,0,s],[0,1,0],[-s,0,c]])

def Rz(psi):
    c,s = np.cos(psi), np.sin(psi)
    return np.array([[c,-s,0],[s,c,0],[0,0,1]])

# rotate body model from +Z to +X: rotate +90° about Y
R0 = Ry(np.pi/2)

def apply_initial_rotation(px, py, pz, R0):
    pts = np.vstack([px.flatten(), py.flatten(), pz.flatten()])
    pts = (R0 @ pts).T
    return pts[:,0].reshape(px.shape), pts[:,1].reshape(py.shape), pts[:,2].reshape(pz.shape)

# Define a simple rocket model in body frame
def rocket_model():
    rocket_scale = 100
    L, Rb, Hn = 4.0*rocket_scale, 0.2*rocket_scale, 1.0*rocket_scale   # body length, radius, nose height
    fin_w, fin_h = 0.1*rocket_scale, 0.5*rocket_scale     # fin width, height

    # Cylinder body along z-axis
    theta = np.linspace(0, 2*np.pi, 16)
    z = np.linspace(0, L, 2)
    theta, z = np.meshgrid(theta, z)
    x = Rb * np.cos(theta)
    y = Rb * np.sin(theta)
    z = z


    # Nose cone
    cone_z = np.linspace(L, L + Hn, 2)
    cone_r = np.linspace(Rb, 0, 2)
    theta = np.linspace(0, 2*np.pi, 16)
    theta, cone_z = np.meshgrid(theta, cone_z)
    cone_x = (cone_r[:, None] * np.cos(theta)).reshape(2, -1)
    cone_y = (cone_r[:, None] * np.sin(theta)).reshape(2, -1)
    cone_z = cone_z


    # Fins (4 at 90 deg)
    fins = []
    for ang in [0, np.pi/2, np.pi, 3*np.pi/2]:
        dx, dy = np.cos(ang), np.sin(ang)
        fin = np.array([
            [Rb*dx, Rb*dy, 0],
            [(Rb+fin_w)*dx, (Rb+fin_w)*dy, 0],
            [(Rb+fin_w)*dx, (Rb+fin_w)*dy, fin_h],
            [Rb*dx, Rb*dy, fin_h]
        ])
        fins.append(fin)


    return (x, y, z, cone_x, cone_y, cone_z, fins)

x, y, z, cone_x, cone_y, cone_z, fins = rocket_model()

x, y, z = apply_initial_rotation(x, y, z, R0)
cone_x, cone_y, cone_z = apply_initial_rotation(cone_x, cone_y, cone_z, R0)
fins = [ ( (R0 @ f.T).T ) for f in fins ]  # convert each fin similarly (then reshape if needed)

# Animation setup
fig = plt.figure()
ax = fig.add_subplot(111, projection="3d")

ax.set_xlim(np.min(pos[:,0]), np.max(pos[:,0]))
ax.set_ylim(np.min(pos[:,1]), np.max(pos[:,1]))
ax.set_zlim(np.min(pos[:,2]), np.max(pos[:,2]))

body = ax.plot_surface(x, y, z, color="red", alpha=0.7)
cone = ax.plot_surface(cone_x, cone_y, cone_z, color="darkred", alpha=0.8)
fin_polys = [ax.add_collection3d(Poly3DCollection([f], color="black")) for f in fins]
traj, = ax.plot([], [], [], "b--")

# Update function
def update(frame):
    ax.collections.clear()  # clear fins each frame

    # Rotation matrix from roll, pitch, yaw
    rot = R.from_euler("xyz", [roll[frame], pitch[frame], yaw[frame]])

    # Transform body, cone, fins
    def transform_points(px, py, pz):
        pts = np.vstack([px.flatten(), py.flatten(), pz.flatten()]).T
        pts = rot.apply(pts) + pos[frame]
        return pts[:,0].reshape(px.shape), pts[:,1].reshape(py.shape), pts[:,2].reshape(pz.shape)

    bx, by, bz = transform_points(x, y, z)
    cx, cy, cz = transform_points(cone_x, cone_y, cone_z)

    ax.plot_surface(bx, by, bz, color="red", alpha=0.7)
    ax.plot_surface(cx, cy, cz, color="darkred", alpha=0.8)

    for f in fins:
        pts = rot.apply(f) + pos[frame]
        poly = Poly3DCollection([pts], color="black")
        ax.add_collection3d(poly)

    # Draw trajectory up to this point
    traj.set_data(pos[:frame,0], pos[:frame,1])
    traj.set_3d_properties(pos[:frame,2])


    return []

ani = FuncAnimation(fig, update, frames=len(t), interval=50, blit=False)
plt.show()
