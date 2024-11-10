import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib import font_manager as font_manager

# Customise plots to look consistent.
mpl.rc("lines", linewidth=0.5)
mpl.rc("axes", labelsize=8, titlesize=12, linewidth=0.2)
mpl.rc("legend", fontsize=8)
mpl.rc("figure", dpi=200)
mpl.rc("xtick", labelsize=8)
mpl.rc("xtick.major", size=2, width=0.5)
mpl.rc("xtick.minor", size=1, width=0.25, visible=True)
mpl.rc("ytick", labelsize=8)
mpl.rc("ytick.major", size=2, width=0.5)
mpl.rc("ytick.minor", size=1, width=0.25, visible=True)

# We first estimate our exhaust temperature.
# Good open source tool here, this is where we get gamma max from.
# https://proptools.readthedocs.io/en/latest/solid_tutorial.html

# From https://www.thrustcurve.org/simfiles/5f9244e11bca580004171750/
# We use the certified values.

# We can also measure our nozzle dimensions and use RocketCEA with a monoprop:
# https://rocketcea.readthedocs.io/en/latest/propellants.html#ap10-rdx25-htpb

burn_time_s = 1.9
prop_weight_kg = 355 / 1000
avg_thrust_N = 280
total_impulse_Ns = 561

# First we estimate mass flow rate:
mfr_kg_s = prop_weight_kg / burn_time_s

# F = m_dot V_e, so exit velocity is F / m_dot
# Note, this is sea level thrust, for P_e - P_0 = 0.
v_exit = avg_thrust_N / mfr_kg_s
print("v_exit", v_exit)

# Now we can find the mach number at the nozzle with:
# M_exit = v_exit / M_sea_level
M_sea_level = 340.3  # [m/s]
M_exit = v_exit / M_sea_level
print("M_exit", M_exit)

# We use the decomposition process as outlined in /resources/ap_characteristics
T_chamber = 1205 - 273.15

# Now use isentropic nozzle flow equation for exit temp:
# T = T_chamber(1+ ((gamma-1)/2)*M_exit**2)**(-1)
gamma = np.linspace(
    1.26, 1, 50
)  # Ive read a ton of different values for this, lets use a range.
T = T_chamber / (1 + (((gamma - 1) / 2) * (M_exit**2)))

print("gamma", gamma)
print("mean exit temp", T)

fig, ax = plt.subplots(constrained_layout=True)

ax.grid(c="grey", linestyle="--", zorder=0)
ax.plot(
    gamma, T, label="Nozzle exit temperature" + r"[$^{\circ}C$]", c="red", zorder=10
)
ax.set_xlabel(r"Ratio of specific heats, $\gamma$")
ax.set_ylabel(r"Temperature [$^{\circ}C$]")
ax.set_title("Solid motor thermal properties.")

ax.legend()

plt.savefig("nozzle_exit_temp.png")
