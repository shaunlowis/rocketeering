import matplotlib.pyplot as plt
import matplotlib.animation as animation
from itertools import count
import random  # For simulating data, replace with actual data source

# plt.style.use('fivethirtyeight')

# xtest = [1, 2, 3, 4, 5, 6]
# ytest = [4, 3, 2, 5, 1, 2]
# index = count()

# def animate(i):
#     xtest.append(next(index))
#     ytest.append(random.randint(0, 5))
#     xtest.pop(0)
#     ytest.pop(0)
#     plt.plot(xtest, ytest)

# ani = animation.FuncAnimation(plt.gcf(), animate, interval=100)

# plt.plot(xtest, ytest)
# plt.tight_layout()
# plt.show()
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import random  # For simulating data, replace with actual data source

# Initialize the figure and axis
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=2)

# Set axis limits
ax.set_ylim(0, 10)  # Adjust y-axis range according to your data

# Data storage
xdata, ydata = [], []

# Function to initialize the plot
def init():
    line.set_data([], [])
    return line,

# Function to update the plot with new data
def update(frame):
    # Simulate new sensor data (replace with actual sensor data)
    new_value = random.uniform(0, 10)  # Simulated sensor data
    xdata.append(frame)  # Use frame as the x-value, or replace with actual x-value (e.g., time)
    ydata.append(new_value)

    # Keep only the last 100 points
    if len(xdata) > 100:
        xdata.pop(0)
        ydata.pop(0)

    # Update the data of the plot
    line.set_data(xdata, ydata)

    # Dynamically shift x-axis window, but only once data exceeds 100 points
    ax.set_xlim(xdata[0], xdata[-1])  # Show only the current window of data
    return line,

# Create an animation object
ani = animation.FuncAnimation(fig, update, frames=range(1000), init_func=init,
                              blit=True, interval=100)

# Show the plot
plt.show()
