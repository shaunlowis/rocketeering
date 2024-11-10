# In nozzle_exit_temp.py, we have a Tmax of 932 deg C
# The heat transfer diffusion equation is given by:
# d/dt(rho*C_p*phi) = grad [k * grad phi]
# For phi = temp, density = rho, heat capacity = C_p and
# thermal conductivity = k

# For Stainless 430, we use:
rho = 7750  # [kg/m3]
k = 21.5  # [W/mK], @ Temperature > 500 C
C_p = 460.548  # [J/kgK],
phi_K = 1205.15  # [K]

# Which we rewrite D with alpha as:
# D = alpha = k/rho * C_p
D = k / (rho * C_p)

# Now our fipy sim. We get the diffusion format from here:
# https://www.ctcms.nist.gov/fipy/examples/diffusion/generated/examples.diffusion.mesh1D.html#module-examples.diffusion.mesh1D
# Then convert to 2D from here:
# https://www.ctcms.nist.gov/fipy/examples/diffusion/generated/examples.diffusion.mesh20x20.html
from fipy import CellVariable, Grid2D, Viewer, TransientTerm, DiffusionTerm, input
from fipy.tools import numerix

# Makes plots look better.
import mpl_rc

# We can update these later
nx = 20
ny = nx
dx = 1.0
dy = dx
L = dx * nx
mesh = Grid2D(dx=dx, dy=dy, nx=nx, ny=ny)

# D = 1.0

# Boundary conditions:
valueLeft = 1
valueRight = 0

valueTopLeft = 0
valueBottomRight = 1

X, Y = mesh.faceCenters
facesTopLeft = (mesh.facesLeft & (Y > L / 2)) | (mesh.facesTop & (X < L / 2))
facesBottomRight = (mesh.facesRight & (Y < L / 2)) | (mesh.facesBottom & (X > L / 2))

# Just set this constraint to be very high
fluxRight = 999.0

phiT = CellVariable(name="Temperature [K]", mesh=mesh, value=phi_K)
phiT.faceGrad.constrain([fluxRight], mesh.facesRight)

phiT.constrain(valueTopLeft, facesTopLeft)
# phiT.constrain(valueBottomRight, facesBottomRight)

# k = 1.0
# X = mesh.faceCenters[0]
eqT = DiffusionTerm(coeff=D)
# eqT.solve(var=phiT)

timeStepDuration = 10 * 0.9 * dx**2 / (2 * D)
steps = 2
from builtins import range

for step in range(steps):
    eqT.solve(var=phiT, dt=timeStepDuration)
    if __name__ == "__main__":
        viewer = Viewer(vars=phiT, title="Basic (wrong) heat conduction.")
        fig_out = viewer.fig
        fig_out.savefig("test.png")
        # viewer.plot()

print(numerix.allclose(phiT(((L,), (0,))), valueBottomRight, atol=1e-2))

if __name__ == "__main__":
    input("Implicit transient diffusion. Press <return> to proceed...")
