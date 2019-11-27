#!/usr/bin/env python3

import pharein as ph

# configure the simulation

ph.Simulation(
    time_step_nbr=1000,        # number of time steps (not specified if time_step and final_time provided)
    final_time=1.,             # simulation final time (not specified if time_step and time_step_nbr provided)
    boundary_types="periodic", # boundary condition, string or tuple, length == len(cell) == len(dl)
    cells=65,                  # integer or tuple length == dimension
    dl=1./65,                    # mesh size of the root level, float or tuple
    max_nbr_levels=2,          # (default=1) max nbr of levels in the AMR hierarchy
    refinement_boxes = {"L0":{"B0":[(10,),(50,)]}}

  # time_step = 0.005,         # simulation time step (not specified if time_step_nbr and final_time provided)
  # domain_size = 8.,          # float or tuple, not specified if dl and cells are
  # interp_order = 1,          # interpolation order, [default = 1] can be 1, 2, 3 or 4
  # layout = "yee"                      # grid layout, [default="yee"]
  # origin = 0.,                        # position of the origin of the domain, float or tuple (length = dimension)
  # particle_pusher = "modified_boris", # particle pusher method, [default = "modified_boris"]
  # refined_particle_nbr = 2,           # number of refined particle a particle is split into [default : ]
  # diag_export_format = 'ascii',       # export format of the diagnostics [default = 'ascii']
  #, refinement = {"level":[0,1],       # AMR parameters
  #                "extent_ratio":[0.4, 0.6],
  #                "refinement_iterations":[0, 3]},

)


# configure the model for the initial condition
ph.UniformModel(proton1={},
                proton2={"density":2,
                         "vbulk":(1., 0., 0.)}

                # demo_species = {density: 2,           # default = 1
                #                 vbulk: (10,0,0),      # default = (0., 0., 0.)
                #                 charge: 1,            # default = 1
                #                 mass: 16,             # default = 1
                #                 beta: 0.05,           # default = 1
                #                 anisotropy=1          # default = 1 (Tperp/Tpara)
                #                 }
)






