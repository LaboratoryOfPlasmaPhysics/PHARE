import numpy as np

from pyphare.core import box as boxm
from pyphare.core.gridlayout import GridLayout
from pyphare.pharesee.hierarchy import FieldData


# only supports ndim == 1, updates needed for > 1
# assumes refinement_ratio is 2


def refine(field, **kwargs):
    """
    optional kwargs
      data : for overriding the field dataset so you don't have to make a temporary copy of a field just to have a different dataset
    """
    assert isinstance(field, FieldData)
    kwarg_keys = ["data"]
    assert any([key in kwarg_keys for key in list(kwargs.keys())])

    refinement_ratio = 2
    primal_directions = field.primal_directions()

    data = kwargs.get("data", field.dataset[:])
    if "data" in kwargs:
        assert data.shape == field.dataset.shape

    fine_box = boxm.refine(field.box, refinement_ratio)
    fine_layout = GridLayout(
        fine_box,
        field.origin,
        field.layout.dl / refinement_ratio,
        interp_order=field.layout.interp_order,
    )
    fine_data = np.zeros(fine_box.shape + primal_directions + (field.ghosts_nbr * 2))

    ghostX = field.ghosts_nbr[0]
    assert ghostX > 0

    cadence = 2

    if field.box.ndim == 1:
        if primal_directions[0]:
            fine_data[ghostX:-ghostX:cadence] = data[
                ghostX:-ghostX
            ]  # coarse primal on top of fine
            fine_data[ghostX + 1 : -(ghostX - 1) : cadence] = (
                0.5 * data[ghostX:-ghostX] + 0.5 * data[ghostX + 1 : -(ghostX - 1)]
            )
        else:
            fine_data[ghostX : -(ghostX + 1) : cadence] = (
                0.25 * data[ghostX - 1 : -(ghostX + 1)] + 0.75 * data[ghostX:-ghostX]
            )
            fine_data[ghostX + 1 : -(ghostX - 1) : cadence] = (
                0.25 * data[ghostX + 1 : -(ghostX - 1)] + 0.75 * data[ghostX:-ghostX]
            )

    if fine_box.ndim > 1:
        ghostY = field.ghosts_nbr[1]
        gX = ghostX  # minify
        gY = ghostY
        cad = cadence

    if fine_box.ndim == 2:
        if all(primal_directions):
            fine_data[gX:-gX:cad, gY:-gY:cad] = data[gX:-gX, gY:-gY]
            fine_data[gX + 1 : -(gX - 1) : cad, gY:-gY:cad] = (
                0.5 * data[gX:-gX, gY:-gY] + 0.5 * data[gX + 1 : -(gX - 1), gY:-gY]
            )
            fine_data[gX:-gX:cad, gY + 1 : -(gY - 1) : cad] = (
                0.5 * data[gX:-gX, gY:-gY] + 0.5 * data[gX:-gX, gY + 1 : -(gY - 1)]
            )
            fine_data[gX + 1 : -(gX - 1) : cad, gY + 1 : -(gY - 1) : cad] = (
                0.25 * data[gX:-gX, gY:-gY]
                + 0.25 * data[gX + 1 : -(gX - 1), gY:-gY]
                + 0.25 * data[gX:-gX, gY + 1 : -(gY - 1)]
                + 0.25 * data[gX + 1 : -(gX - 1), gY + 1 : -(gY - 1)]
            )

        elif primal_directions[0] and not primal_directions[1]:
            fine_data[gX:-gX:cad, gY:-gY:cad] = (
                0.25 * data[gX:-gX, gY - 1 : -(gY + 1)] + 0.75 * data[gX:-gX, gY:-gY]
            )
            fine_data[gX + 1 : -(gX - 1) : cad, gY:-gY:cad] = 0.75 * (
                0.5 * data[gX + 1 : -(gX - 1), gY:-gY] + 0.5 * data[gX:-gX, gY:-gY]
            ) + 0.25 * (
                0.5 * data[gX + 1 : -(gX - 1), gY - 1 : -(gY + 1)]
                + 0.5 * data[gX:-gX, gY - 1 : -(gY + 1)]
            )
            fine_data[gX:-gX:cad, gY + 1 : -(gY - 1) : cad] = (
                0.25 * data[gX:-gX, gY + 1 : -(gY - 1)] + 0.75 * data[gX:-gX, gY:-gY]
            )
            fine_data[gX + 1 : -(gX - 1) : cad, gY + 1 : -(gY - 1) : cad] = 0.75 * (
                0.5 * data[gX + 1 : -(gX - 1), gY:-gY] + 0.5 * data[gX:-gX, gY:-gY]
            ) + 0.25 * (
                0.5 * data[gX + 1 : -(gX - 1), gY + 1 : -(gY - 1)]
                + 0.5 * data[gX:-gX, gY + 1 : -(gY - 1)]
            )

        elif not primal_directions[0] and primal_directions[1]:
            fine_data[gX:-gX:cad, gY:-gY:cad] = (
                0.25 * data[gX - 1 : -(gX + 1), gY:-gY] + 0.75 * data[gX:-gX, gY:-gY]
            )
            fine_data[gX + 1 : -(gX - 1) : cad, gY:-gY:cad] = (
                0.25 * data[gX + 1 : -(gX - 1), gY:-gY] + 0.75 * data[gX:-gX, gY:-gY]
            )
            fine_data[gX:-gX:cad, gY + 1 : -(gY - 1) : cad] = 0.5 * (
                0.25 * data[gX - 1 : -(gX + 1), gY:-gY] + 0.75 * data[gX:-gX, gY:-gY]
            ) + 0.5 * (
                0.25 * data[gX - 1 : -(gX + 1), gY + 1 : -(gY - 1)]
                + 0.75 * data[gX:-gX, gY + 1 : -(gY - 1)]
            )
            fine_data[gX + 1 : -(gX - 1) : cad, gY + 1 : -(gY - 1) : cad] = 0.5 * (
                0.25 * data[gX + 1 : -(gX - 1), gY:-gY] + 0.75 * data[gX:-gX, gY:-gY]
            ) + 0.5 * (
                0.25 * data[gX + 1 : -(gX - 1), gY + 1 : -(gY - 1)]
                + 0.75 * data[gX:-gX, gY + 1 : -(gY - 1)]
            )

        elif not any(primal_directions):
            fine_data[gX:-gX:cad, gY:-gY:cad] = 0.75 * (
                0.25 * data[gX - 1 : -(gX + 1), gY:-gY] + 0.75 * data[gX:-gX, gY:-gY]
            ) + 0.25 * (
                0.25 * data[gX - 1 : -(gX + 1), gY - 1 : -(gY + 1)]
                + 0.75 * data[gX:-gX, gY - 1 : -(gY + 1)]
            )
            fine_data[gX + 1 : -(gX - 1) : cad, gY:-gY:cad] = 0.75 * (
                0.25 * data[gX + 1 : -(gX - 1), gY:-gY] + 0.75 * data[gX:-gX, gY:-gY]
            ) + 0.25 * (
                0.25 * data[gX + 1 : -(gX - 1), gY - 1 : -(gY + 1)]
                + 0.75 * data[gX:-gX, gY - 1 : -(gY + 1)]
            )
            fine_data[gX:-gX:cad, gY + 1 : -(gY - 1) : cad] = 0.75 * (
                0.25 * data[gX - 1 : -(gX + 1), gY:-gY] + 0.75 * data[gX:-gX, gY:-gY]
            ) + 0.25 * (
                0.25 * data[gX - 1 : -(gX + 1), gY + 1 : -(gY - 1)]
                + 0.75 * data[gX:-gX, gY + 1 : -(gY - 1)]
            )
            fine_data[gX + 1 : -(gX - 1) : cad, gY + 1 : -(gY - 1) : cad] = 0.75 * (
                0.25 * data[gX + 1 : -(gX - 1), gY:-gY] + 0.75 * data[gX:-gX, gY:-gY]
            ) + 0.25 * (
                0.25 * data[gX + 1 : -(gX - 1), gY + 1 : -(gY - 1)]
                + 0.75 * data[gX:-gX, gY + 1 : -(gY - 1)]
            )

    return FieldData(fine_layout, field.field_name, data=fine_data)


def refine_time_interpolate(
    datahier,
    quantities,
    coarse_ilvl,
    coarsest_time_before,
    coarsest_time_after,
    fine_subcycle_times,
):
    """
    returns {qty : { subcycle_time: [refined_time_interpolated_fields]}}
    """

    from tests.core.numerics.interpolator.interpolator_test import time_interpolate

    def _sort(patches):
        return sorted(patches, key=lambda p: p.origin.all())

    interpolated_fields = {qty: {} for qty in quantities}

    coarse_before_patches = _sort(
        datahier.level(coarse_ilvl, coarsest_time_before).patches
    )
    coarse_after_patches = _sort(
        datahier.level(coarse_ilvl, coarsest_time_after).patches
    )
    assert len(coarse_before_patches) == len(coarse_after_patches)

    for qty in quantities:
        for fine_subcycle_time in fine_subcycle_times:
            interpolated_fields[qty][fine_subcycle_time] = []
            for coarsePatch_idx in range(len(coarse_before_patches)):
                coarse_before_patch = coarse_before_patches[coarsePatch_idx]
                coarse_after_patch = coarse_after_patches[coarsePatch_idx]
                assert coarse_before_patch.box == coarse_after_patch.box
                coarseBefore_pd = coarse_before_patch.patch_datas[qty]
                coarseAfter_pd = coarse_after_patch.patch_datas[qty]
                interpolated_fields[qty][fine_subcycle_time] += [
                    refine(
                        coarseBefore_pd,
                        data=time_interpolate(
                            coarsest_time_before,
                            coarsest_time_after,
                            fine_subcycle_time,
                            coarseBefore_pd.dataset[:],
                            coarseAfter_pd.dataset[:],
                        ),
                    )
                ]

    return interpolated_fields
