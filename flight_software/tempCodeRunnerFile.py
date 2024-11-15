
# Initialize the figure and axis
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=2)

# Data storage
xdata, ydata = [], []

# Function to initialize the plot
def init():
    line.set_data([], [])
    return line,

# Function to update the plot with new data
def update(frame):
    # Simulate new sensor data (replace this with actual sensor data)
    xdata.append(frame)
    ydata.append(random.uniform(0, 10))  # Simulated sensor data

    # Limit the number of displayed points
    if len(xdata) > 100:
        xdata.pop(0)
        ydata.pop(0)

    # Update the data of the plot
    line.set_data(range(len(xdata)), ydata)
    ax.set_xlim(max(0, frame-100), frame)  # Shift x-axis window to show last 100 points
    return line,

# Create an animation object
ani = animation.FuncAnimation(fig, update, frames=range(200), init_func=init, \
                              blit=True, interval=100)

# Show the plot
plt.show()