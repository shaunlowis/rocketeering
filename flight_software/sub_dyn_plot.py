import matplotlib.pyplot as plt
import matplotlib.animation as animation
import random  # For simulating data, replace with actual data source

# Initialize the figure and 4 subplots (2x2 grid)
fig, axs = plt.subplots(2, 2)

# Initialize line objects for each subplot
# First subplot has two lines, the rest have one line each
lines = [[], [], [], []]  # To hold two lines for the first plot, one for others
lines[0].append(axs[0, 0].plot([], [], lw=2, label='Line 1')[0])  # First line on the first plot
lines[0].append(axs[0, 0].plot([], [], lw=2, label='Line 2', color='red')[0])  # Second line on the first plot
lines[1].append(axs[0, 1].plot([], [], lw=2)[0])  # One line for the second plot
lines[2].append(axs[1, 0].plot([], [], lw=2)[0])  # One line for the third plot
lines[3].append(axs[1, 1].plot([], [], lw=2)[0])  # One line for the fourth plot

# Set axis limits for each subplot
for ax in axs.flat:
    ax.set_xlim(0, 100)  # x-axis for the last 100 points

# Data storage for the last 100 points for each line
xdata = [[] for _ in range(4)]  # Separate x-data for each subplot
ydata = [[] for _ in range(4)]  # Separate y-data for each subplot
ydata2 = []  # Second y-data list for the second line on the first plot

# Function to initialize the plot
def init():
    for sub_lines in lines:
        for line in sub_lines:
            line.set_data([], [])
    return [line for sub_lines in lines for line in sub_lines]

# Function to update the plot with new data
def update(frame):
    # Simulate new sensor data for each subplot (replace with actual sensor data)
    new_values = [random.uniform(0, 10) for _ in range(4)]  # Simulated data for each subplot
    new_value2 = random.uniform(0, 10)  # Additional data for second line on the first subplot

    # Update the first subplot with two lines
    xdata[0].append(frame)
    ydata[0].append(new_values[0])
    if len(ydata2) < len(xdata[0]):
        ydata2.append(new_value2)  # Add data to the second line

    # Keep only the last 100 points for the first subplot
    if len(xdata[0]) > 100:
        xdata[0].pop(0)
        ydata[0].pop(0)
        ydata2.pop(0)

    # Update the first plot with two lines
    lines[0][0].set_data(xdata[0], ydata[0])
    lines[0][1].set_data(xdata[0], ydata2)

    # Automatically adjust ylim based on data range for the first plot
    y_min = min(min(ydata[0]), min(ydata2))
    y_max = max(max(ydata[0]), max(ydata2))
    axs[0, 0].set_ylim(y_min - 1, y_max + 1)  # Add some padding to avoid cutting off data

    # Update the other three subplots with single lines
    for i in range(1, 4):
        xdata[i].append(frame)
        ydata[i].append(new_values[i])

        if len(xdata[i]) > 100:
            xdata[i].pop(0)
            ydata[i].pop(0)

        # Update the single line on each of the other three subplots
        lines[i][0].set_data(xdata[i], ydata[i])

        # Automatically adjust ylim based on data range for each subplot
        axs.flat[i].set_ylim(min(ydata[i]) - 1, max(ydata[i]) + 1)

    # Dynamically adjust x-axis window for each subplot
    for i in range(4):
        axs.flat[i].set_xlim(xdata[i][0], xdata[i][-1])

    return [line for sub_lines in lines for line in sub_lines]

# Create an animation object
ani = animation.FuncAnimation(fig, update, frames=range(1000), init_func=init,
                              blit=True, interval=100)

# Adjust layout for better spacing between subplots
plt.tight_layout()

# Add legend for the first subplot
axs[0, 0].legend()

# Show the plot
plt.show()
