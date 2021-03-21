

def dump_all_diags(pops=[], flush_every=100):
    import pyphare.pharein as ph, numpy as np

    sim = ph.global_vars.sim


    timestamps = np.arange(0, sim.final_time + sim.time_step, sim.time_step)

    for quantity in ["density", "bulkVelocity"]:
        ph.FluidDiagnostics(
            quantity=quantity,
            write_timestamps=timestamps,
            compute_timestamps=timestamps,
            flush_every=flush_every,
        )

    for pop in pops:
        for quantity in ["density", "flux"]:
          ph.FluidDiagnostics(
              quantity=quantity,
              write_timestamps=timestamps,
              compute_timestamps=timestamps,
              flush_every=flush_every,
              population_name=pop
          )

        for quantity in ['domain', 'levelGhost', 'patchGhost']:
            ph.ParticleDiagnostics(
                quantity=quantity,
                compute_timestamps=timestamps,
                write_timestamps=timestamps,
                flush_every=flush_every,
                population_name=pop
            )

    for quantity in ["E", "B"]:
        ph.ElectromagDiagnostics(
            quantity=quantity,
            write_timestamps=timestamps,
            compute_timestamps=timestamps,
            flush_every=flush_every,
        )
