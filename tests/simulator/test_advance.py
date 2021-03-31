
from pybindlibs import cpp

from pyphare.simulator.simulator import Simulator, startMPI
from pyphare.pharesee.hierarchy import hierarchy_from, merge_particles
from pyphare.pharein import MaxwellianFluidModel
from pyphare.pharein.diagnostics import ParticleDiagnostics, FluidDiagnostics, ElectromagDiagnostics
from pyphare.pharein import ElectronModel
from pyphare.pharein.simulation import Simulation
from pyphare.pharesee.geometry import level_ghost_boxes, hierarchy_overlaps
from pyphare.core.gridlayout import GridLayout, yee_element_is_primal
from pyphare.pharesee.particles import aggregate as aggregate_particles
import pyphare.core.box as boxm
from pyphare.core.box import Box, Box1D
import numpy as np
import unittest
from ddt import ddt, data, unpack


# AdvanceTest.test_field_level_ghosts_via_subcycles_and_coarser_interpolation_1

@ddt
class AdvanceTest(unittest.TestCase):

    def ddt_test_id(self):
        return self._testMethodName.split("_")[-1]

    def getHierarchy(self, interp_order, refinement_boxes, qty, nbr_part_per_cell=100,
                     diag_outputs="phare_outputs",
                     smallest_patch_size=5, largest_patch_size=5,
                     cells= 120, time_step=0.001, model_init={},
                     dl=0.3, extra_diag_options={}, time_step_nbr=1, timestamps=None):

        from pyphare.pharein import global_vars
        global_vars.sim = None
        startMPI()
        extra_diag_options["mode"] = "overwrite"
        extra_diag_options["dir"] = diag_outputs
        Simulation(
            smallest_patch_size=smallest_patch_size,
            largest_patch_size=largest_patch_size,
            time_step_nbr=time_step_nbr,
            time_step=time_step,
            boundary_types="periodic",
            cells=cells,
            dl=dl,
            interp_order=interp_order,
            refinement_boxes=refinement_boxes,
            diag_options={"format": "phareh5",
                          "options": extra_diag_options}
        )

        def density(x):
            return 1.

        def S(x,x0,l):
            return 0.5*(1+np.tanh((x-x0)/l))

        def bx(x):
            return 1.

        def by(x):
            L = global_vars.sim.simulation_domain()[0]
            v1=-1
            v2=1.
            return v1 + (v2-v1)*(S(x,L*0.25,1) -S(x, L*0.75, 1))

        def bz(x):
            return 0.5

        def b2(x):
            return bx(x)**2 + by(x)**2 + bz(x)**2

        def T(x):
            K = 1
            return 1/density(x)*(K - b2(x)*0.5)

        def vx(x):
            return 0.

        def vy(x):
            return 0.

        def vz(x):
            return 0.

        def vthx(x):
            return T(x)

        def vthy(x):
            return T(x)

        def vthz(x):
            return T(x)


        MaxwellianFluidModel(bx=bx, by=by, bz=bz,
                             protons={"charge": 1,
                                      "density": density,
                                      "vbulkx": vx, "vbulky": vy, "vbulkz": vz,
                                      "vthx": vthx, "vthy": vthy, "vthz": vthz,
                                      "nbr_part_per_cell": nbr_part_per_cell,
                                      "init": model_init})

        ElectronModel(closure="isothermal", Te=0.12)

        if timestamps is None:
            timestamps = np.arange(0, global_vars.sim.final_time + global_vars.sim.time_step, global_vars.sim.time_step)

        for quantity in ["E", "B"]:
            ElectromagDiagnostics(
                quantity=quantity,
                write_timestamps=timestamps,
                compute_timestamps=timestamps
            )

        for quantity in ["density", "bulkVelocity"]:
            FluidDiagnostics(
                quantity=quantity,
                write_timestamps=timestamps,
                compute_timestamps=timestamps
            )

        for pop in ["protons"]:
            for quantity in ["density", "flux"]:
                FluidDiagnostics(quantity=quantity,
                                 write_timestamps=timestamps,
                                 compute_timestamps=timestamps,
                                 population_name=pop)

        Simulator(global_vars.sim).initialize().run().reset()

        eb_hier = None
        if qty in ["e", "eb"]:
            eb_hier = hierarchy_from(h5_filename=diag_outputs+"/EM_E.h5", hier=eb_hier)
        if qty in ["b", "eb"]:
            eb_hier = hierarchy_from(h5_filename=diag_outputs+"/EM_B.h5", hier=eb_hier)
        if qty in ["e", "b", "eb"]:
            return eb_hier

        if qty == "moments":
            mom_hier = hierarchy_from(h5_filename=diag_outputs+"/ions_density.h5")
            mom_hier = hierarchy_from(h5_filename=diag_outputs+"/ions_bulkVelocity.h5", hier=mom_hier)
            mom_hier = hierarchy_from(h5_filename=diag_outputs+"/ions_pop_protons_density.h5", hier=mom_hier)
            mom_hier = hierarchy_from(h5_filename=diag_outputs+"/ions_pop_protons_flux.h5", hier=mom_hier)
            return mom_hier




    def _test_field_coarsening_via_subcycles(self, dim, interp_order, refinement_boxes):
        print("test_field_coarsening_via_subcycles for dim/interp : {}/{}".format(dim, interp_order))

        from tests.amr.data.field.coarsening.test_coarsen_field import coarsen
        from pyphare.pharein import global_vars

        time_step_nbr=3

        diag_outputs=f"phare_outputs_subcycle_coarsening_{self.ddt_test_id()}"
        datahier = self.getHierarchy(interp_order, refinement_boxes, "eb", cells=30,
                                      diag_outputs=diag_outputs, time_step=0.001,
                                      extra_diag_options={"fine_dump_lvl_max": 10},
                                      time_step_nbr=time_step_nbr, smallest_patch_size=5,
                                      largest_patch_size=30)

        lvl_steps = global_vars.sim.level_time_steps
        print("LEVELSTEPS === ", lvl_steps)
        assert len(lvl_steps) > 1, "this test makes no sense with only 1 level"

        finestTimeStep = lvl_steps[-1]
        secondFinestTimeStep = lvl_steps[-2]

        finest_level_step_nbr = global_vars.sim.level_step_nbr[-1]
        uniqTimes = set([0])

        for step in range(1, finest_level_step_nbr + 1):
            checkTime = datahier.format_timestamp(finestTimeStep * step)
            self.assertIn(checkTime, datahier.times())
            uniqTimes.add(checkTime)

        self.assertEqual(len(uniqTimes), len(datahier.time_hier.items()))

        syncSteps = global_vars.sim.level_step_nbr[-2] # ignore finest subcycles

        # FIX THIS AFTER NO MORE REGRIDS
        #  SEE: https://github.com/PHAREHUB/PHARE/issues/400
        assert syncSteps % time_step_nbr == 0 # perfect division
        startStep = int(syncSteps / time_step_nbr) + 1 # skip first coarsest step due to issue 400

        for step in range(startStep, syncSteps + 1):
            checkTime = datahier.format_timestamp(secondFinestTimeStep * step)
            self.assertIn(checkTime, datahier.times())
            nLevels = datahier.levelNbr(checkTime)
            self.assertGreaterEqual(nLevels, 2)
            levelNbrs = datahier.levelNbrs(checkTime)
            finestLevelNbr = max(levelNbrs)
            coarsestLevelNbr = min(levelNbrs)

            for coarseLevelNbr in range(coarsestLevelNbr, finestLevelNbr):
                coarsePatches = datahier.level(coarseLevelNbr, checkTime).patches
                finePatches = datahier.level(coarseLevelNbr + 1, checkTime).patches

                for coarsePatch in coarsePatches:
                    for finePatch in finePatches:
                        lvlOverlap = boxm.refine(coarsePatch.box, 2) * finePatch.box
                        if lvlOverlap is not None:
                            for EM in ["E", "B"]:
                                for xyz in ["x", "y", "z"]:
                                    qty = f"{EM}{xyz}"
                                    coarse_pd = coarsePatch.patch_datas[qty]
                                    fine_pd  = finePatch.patch_datas[qty]
                                    coarseBox = boxm.coarsen(lvlOverlap, 2)

                                    nGhosts = coarse_pd.layout.nbrGhostFor(qty)

                                    coarse_pdDataset = coarse_pd.dataset[:]
                                    fine_pdDataset = fine_pd.dataset[:]

                                    coarseOffset = coarseBox.lower - coarse_pd.layout.box.lower
                                    dataBox_lower = coarseOffset + nGhosts
                                    dataBox = Box(dataBox_lower, dataBox_lower + coarseBox.shape - 1)

                                    afterCoarse = np.copy(coarse_pdDataset)
                                    afterCoarse[dataBox.lower[0] : dataBox.upper[0] + 1] = 0

                                    coarsen(qty, coarse_pd.layout, fine_pd.layout, coarseBox, fine_pdDataset, afterCoarse)
                                    np.testing.assert_allclose(coarse_pdDataset, afterCoarse, atol=1e-6)


    @data(
       ({"L0": {"B0": Box1D(10, 19)}}),
       ({"L0": {"B0": Box1D(10, 14), "B1": Box1D(15, 19)}}),
       ({"L0": {"B0": Box1D(6, 23)}}),
       ({"L0": {"B0": Box1D( 2, 12), "B1": Box1D(13, 25)}}),
       ({"L0": {"B0": Box1D( 5, 20)}, "L1": {"B0": Box1D(15, 19)}}),
       ({"L0": {"B0": Box1D( 5, 20)}, "L1": {"B0": Box1D(12, 38)}, "L2": {"B0": Box1D(30, 52)} }),
    )
    def test_field_coarsening_via_subcycles(self, refinement_boxes):
        dim = refinement_boxes["L0"]["B0"].ndim
        self._test_field_coarsening_via_subcycles(dim, interp_order=1, refinement_boxes=refinement_boxes)




    def _test_field_level_ghosts_via_subcycles_and_coarser_interpolation(self, ndim, interp_order, refinement_boxes):
        """
          This test runs two virtually identical simulations for one step.
            L0_datahier has no refined levels
            L0L1_datahier has one refined level

          This is done to compare L0 values that haven't received the coarsened values of L1 because there is no L1,
            to the level field ghost of L1 of L0L1_datahier

          The simulations are no longer comparable after the first advance, so this test cannot work beyond that.
        """

        print("test_field_coarsening_via_subcycles for dim/interp : {}/{}".format(ndim, interp_order))

        from tests.amr.data.field.refine.test_refine_field import refine_time_interpolate
        from pyphare.pharein import global_vars

        import random
        rando = random.randint(0, 1e10)

        def _getHier(diag_dir, boxes=[]):
            return self.getHierarchy(interp_order, boxes, "eb", cells=30,
                time_step_nbr=1, smallest_patch_size=5, largest_patch_size=30,
                diag_outputs=diag_dir, extra_diag_options={"fine_dump_lvl_max": 10}, time_step=0.001,
                model_init={"seed": rando}
            )

        def assert_time_in_hier(*ts):
            for t in ts:
                self.assertIn(L0L1_datahier.format_timestamp(t), L0L1_datahier.times())

        L0_datahier = _getHier(f"phare_lvl_ghost_interpolation_L0_diags_{self.ddt_test_id()}")
        L0L1_datahier = _getHier(
          f"phare_lvl_ghost_interpolation_L0L1_diags_{self.ddt_test_id()}", refinement_boxes
        )

        lvl_steps = global_vars.sim.level_time_steps
        assert len(lvl_steps) == 2, "this test is only configured for L0 -> L1 refinement comparisons"

        coarse_ilvl = 0
        fine_ilvl   = 1
        coarsest_time_before = 0 # init
        coarsest_time_after = coarsest_time_before + lvl_steps[coarse_ilvl]
        assert_time_in_hier(coarsest_time_before, coarsest_time_after)

        fine_subcycle_times = []
        for fine_subcycle in range(global_vars.sim.level_step_nbr[fine_ilvl] + 1):
            fine_subcycle_time   = coarsest_time_before + (lvl_steps[fine_ilvl] * fine_subcycle)
            assert_time_in_hier(fine_subcycle_time)
            fine_subcycle_times += [fine_subcycle_time]
            for patch in L0L1_datahier.level(fine_ilvl, fine_subcycle_time).patches:
                for qty, patch_data in patch.patch_datas.items():
                    if yee_element_is_primal(qty): # ignore border nodes for level ghost comparison
                        patch.box = boxm.grow(patch.box, 1)  # invalidates primal fields for everything else but this test

        quantities = [f"{EM}{xyz}" for EM in ["E", "B"] for xyz in ["x", "y", "z"]]
        interpolated_fields = refine_time_interpolate(
          L0_datahier, quantities, coarse_ilvl, coarsest_time_before, coarsest_time_after, fine_subcycle_times
        )

        checks = 0
        for fine_subcycle_time in fine_subcycle_times:
            fine_level_qty_ghost_boxes = level_ghost_boxes(L0L1_datahier, quantities, fine_ilvl, fine_subcycle_time)
            for qty in quantities:
                qty_is_primal = yee_element_is_primal(qty)
                for fine_level_ghost_box_data in fine_level_qty_ghost_boxes[qty]:
                    fine_subcycle_pd = fine_level_ghost_box_data["pdata"]
                    for fine_level_ghost_box in fine_level_ghost_box_data["boxes"]:
                        upper_dims = fine_level_ghost_box.lower > fine_subcycle_pd.box.upper
                        for refinedInterpolatedField in interpolated_fields[qty][fine_subcycle_time]:
                            lvlOverlap = refinedInterpolatedField.box * fine_level_ghost_box
                            if lvlOverlap is not None:

                                fine_ghostbox_data = fine_subcycle_pd[fine_level_ghost_box]
                                refinedInterpGhostBox_data = refinedInterpolatedField[fine_level_ghost_box]

                                fine_ds = fine_subcycle_pd.dataset
                                if fine_level_ghost_box.ndim == 1: # verify selecting start/end of L1 dataset from ghost box
                                    if upper_dims[0]:
                                        assert all(fine_ghostbox_data == fine_ds[-fine_ghostbox_data.shape[0]:])
                                    else:
                                        assert all(fine_ghostbox_data == fine_ds[:fine_ghostbox_data.shape[0]])

                                assert refinedInterpGhostBox_data.shape == fine_subcycle_pd.ghosts_nbr
                                assert fine_ghostbox_data.shape == fine_subcycle_pd.ghosts_nbr
                                np.testing.assert_allclose(fine_ghostbox_data, refinedInterpGhostBox_data, atol=1e-7)
                                checks += 1

        self.assertGreater(checks, len(refinement_boxes["L0"]) * len(quantities))



    @data( # only supports a hierarchy with 2 levels
       ({"L0": [Box1D(5, 9)]}),
       ({"L0": [Box1D(5, 24)]}),
       ({"L0": [Box1D(5, 9), Box1D(20, 24)]}),
    )
    def test_field_level_ghosts_via_subcycles_and_coarser_interpolation(self, refinement_boxes):
        dim = refinement_boxes["L0"][0].ndim
        for interp in [1, 2, 3]:
            self._test_field_level_ghosts_via_subcycles_and_coarser_interpolation(dim, interp, refinement_boxes)


    @data(
       ({"L0": {"B0": Box1D(10, 14), "B1": Box1D(15, 19)}}),
    )
    def test_hierarchy_timestamp_cadence(self, refinement_boxes):
        dim = refinement_boxes["L0"]["B0"].ndim

        time_step     = .001
        # time_step_nbr chosen to force diagnostics dumping double imprecision cadence calculations accuracy testing
        time_step_nbr = 101
        final_time    = time_step * time_step_nbr

        for trailing in [0, 1]: # 1 = skip init dumps
            for i in [2, 3]:
                timestamps = np.arange(0, final_time, time_step*i)[trailing:]

                diag_outputs=f"phare_outputs_hierarchy_timestamp_cadence_{self.ddt_test_id()}_{i}"
                hier = self.getHierarchy(interp_order=1, refinement_boxes=refinement_boxes, qty="eb", cells=30,
                                              diag_outputs=diag_outputs, time_step=time_step,
                                              time_step_nbr=time_step_nbr, smallest_patch_size=5,
                                              largest_patch_size=30, timestamps=timestamps)

                time_hier_keys = list(hier.time_hier.keys())
                self.assertEqual(len(time_hier_keys), len(timestamps))

                for i, timestamp in enumerate(time_hier_keys):
                    self.assertEqual(hier.format_timestamp(timestamps[i]), timestamp)


if __name__ == "__main__":
    unittest.main()
