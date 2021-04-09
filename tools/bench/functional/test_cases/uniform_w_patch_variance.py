"""

  This test case is for assessing the impact of copy/stream for various patch size and parameter sets

"""

import os
import numpy as np
from pathlib import Path
from pyphare.pharein.simulation import supported_dimensions
this_file_name, file_ext = os.path.splitext(os.path.basename(__file__))
gen_path = os.path.join(os.path.dirname(__file__), f'../generated/{this_file_name}')
#### DO NOT EDIT ABOVE ####


### test permutation section - minimized is best ###
dl = 0.25
final_time = 1
mpirun_Ns = [1, 10]
patch_sizes = [5, 10, 50, 100, 200, 400, 800, 1600]
time_step = .01
cells = 1600
ppc_list = [111, 1111, 2222, 3333]
interps = [1, 2, 3]
ndims = supported_dimensions()
vth = { f"vth{xyz}" : lambda *xyz: .3 for xyz in "xyz"}

def generate(ndim, interp, ppc, mpirun_n, patch_size):
    """
      Params may include functions for the default population "protons"
         see: simulation_setup.py::setup for all available dict keys

         simulation_setup.setup doesn't even have to be used, any job.py style file is allowed
         A "params" dict must exist for exporting test case information
    """
    file_name = f"{ndim}_{interp}_{ppc}_{mpirun_n}_{patch_size}"
    with open(os.path.join(gen_path, file_name + ".py"), "w") as out:
        out.write("""
import numpy as np
import tools.bench.functional.test_cases.uniform_w_patch_variance as inputs # scary

params = {""" + f"""
    "mpirun_n"            : {mpirun_n},
    "ndim"                : {ndim},
    "interp_order"        : {interp},
    "ppc"                 : {ppc},
    "smallest_patch_size" : {patch_size},
    "largest_patch_size"  : {patch_size},
    "cells"               : inputs.cells,
    "time_step"           : inputs.time_step,
    "dl"                  : inputs.dl,
    "final_time"          : inputs.final_time,
    **inputs.vth,
""" + """
}

import pyphare.pharein as ph
if ph.PHARE_EXE: # needed to allow params export without calling "job.py"
    from tools.bench.functional.simulation_setup import setup
    setup(**params) # basically a "job.py"

"""     )


### following function is called during test_case generation ###
def generate_all(clean=True):
    gen_dir = Path(gen_path)
    if clean and os.path.exists(gen_path):
        import shutil
        shutil.rmtree(str(gen_dir))
    gen_dir.mkdir(parents=True, exist_ok=True)
    import itertools
    permutations = itertools.product(ndims, interps, ppc_list, mpirun_Ns, patch_sizes)
    for permutation in permutations:
        generate(*permutation)


if __name__ == "__main__":
    generate_all()
