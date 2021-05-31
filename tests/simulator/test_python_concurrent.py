
import os
import unittest

from concurrencytest import ConcurrentTestSuite, fork_for_tests

from tests.simulator.test_validation import SimulatorValidation

from tests.simulator.initialize.test_fields_init_1d import InitializationTest as InitField1d
from tests.simulator.initialize.test_particles_init_1d import InitializationTest as InitParticles1d

from tests.simulator.advance.test_fields_advance_1d import AdvanceTest as AdvanceField1d
from tests.simulator.advance.test_particles_advance_1d import AdvanceTest as AdvanceParticles1d

from tests.simulator.initialize.test_fields_init_2d import InitializationTest as InitField2d
from tests.simulator.initialize.test_particles_init_2d import InitializationTest as InitParticles2d

from tests.simulator.advance.test_fields_advance_2d import AdvanceTest as AdvanceField2d
from tests.simulator.advance.test_particles_advance_2d import AdvanceTest as AdvanceParticles2d

if __name__ == "__main__":

    test_classes_to_run = [
      SimulatorValidation,
      InitField1d,
      InitParticles1d,
      AdvanceField1d,
      AdvanceParticles1d,
      InitField2d,
      InitParticles2d,
      AdvanceField2d,
      AdvanceParticles2d
    ]

    loader = unittest.TestLoader()

    suite = unittest.TestSuite()
    for test_class in test_classes_to_run:
        suite.addTest(loader.loadTestsFromTestCase(test_class))

    import multiprocessing

    N_CORES = int(os.environ["N_CORES"]) if "N_CORES" in os.environ else multiprocessing.cpu_count()
    unittest.TextTestRunner().run(ConcurrentTestSuite(suite, fork_for_tests(N_CORES)))
