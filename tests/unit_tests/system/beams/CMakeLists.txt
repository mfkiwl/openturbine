# Specify the source files for the unit test executable
target_sources(
    openturbine_unit_tests
    PRIVATE
    test_calculate_jacobian.cpp
    test_curved_beam.cpp
    test_integrate_stiffness_matrix.cpp
    test_integrate_inertia_matrix.cpp
    test_integrate_residual_vector.cpp
    test_rotate_section_matrix.cpp
    test_calculate_strain.cpp
    test_calculate_temporary_variables.cpp
    test_calculate_force_FC.cpp
    test_calculate_force_FD.cpp
    test_calculate_Ouu.cpp
    test_calculate_Puu.cpp
    test_calculate_Quu.cpp
)
