#!/usr/bin/env python3

import os, sys, math
import pyphare.pharein as ph
from pyphare.pharein import ElectronModel

print("start test_messengers: input_1d_ratio_2.py")
print("sys.path ", sys.path)

ph.Simulation(
    smallest_patch_size=10,
    largest_patch_size=64,
    time_step_nbr=1000,        # number of time steps (not specified if time_step and final_time provided)
    final_time=1.,             # simulation final time (not specified if time_step and time_step_nbr provided)
    boundary_types="periodic", # boundary condition, string or tuple, length == len(cell) == len(dl)
    cells=65,                  # integer or tuple length == dimension
    dl=1./65,                  # mesh size of the root level, float or tuple
    max_nbr_levels=2,          # (default=1) max nbr of levels in the AMR hierarchy
    refinement_boxes = {"L0":{"B0":[(10,),(50,)]}},
    diag_options = {"format":"phareh5", "options": {"dir": "phare_outputs"}}
)

density = lambda x: 2.

# bx, by, bz = (lambda x: math.cos(2 * math.pi * x) for i in range(3))
# ex, ey, ez = (lambda x: math.cos(2 * math.pi * x) for i in range(3))
bx, by, bz = (lambda x: x if x != 0 else 1 for i in range(3))
ex, ey, ez = (lambda x: x if x != 0 else 1 for i in range(3))
vx, vy, vz = (lambda x: 1. for i in range(3))

vthx, vthy, vthz = (lambda x: 1. for i in range(3))

vvv = {
    "vbulkx":vx, "vbulky":vy, "vbulkz":vz,
    "vthx":vthx, "vthy":vthy, "vthz":vthz
}

ph.MaxwellianFluidModel(
    bx=bx, by=by, bz=bz,
    ex=ex, ey=ey, ez=ez,

    protons={"charge":-1, "density":density, **vvv},
    alpha={"charge":-1, "density":density, **vvv}
)

ElectronModel(closure="isothermal",Te = 0.12)


print("end test_messengers: input_1d_ratio_2.py")
