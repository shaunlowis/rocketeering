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
