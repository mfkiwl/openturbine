#include <Kokkos_Core.hpp>
#include <gtest/gtest.h>

#include "constraints/calculate_rigid_joint_constraint.hpp"

namespace openturbine::tests {

struct ExecuteCalculateRigidJointConstraint {
    Kokkos::View<double[3]>::const_type X0;
    Kokkos::View<double[7]>::const_type base_node_u;
    Kokkos::View<double[7]>::const_type target_node_u;
    Kokkos::View<double[6]> residual_terms;
    Kokkos::View<double[6][6]> base_gradient_terms;
    Kokkos::View<double[6][6]> target_gradient_terms;

    KOKKOS_FUNCTION
    void operator()(int) const {
        CalculateRigidJointConstraint(
            X0, base_node_u, target_node_u, residual_terms, base_gradient_terms,
            target_gradient_terms
        );
    }
};

TEST(CalculateRigidJointConstraintTests, OneConstraint) {
    const auto X0 = Kokkos::View<double[3]>("X0");
    constexpr auto X0_host_data = std::array{1., 2., 3.};
    const auto X0_host = Kokkos::View<double[3], Kokkos::HostSpace>::const_type(X0_host_data.data());
    const auto X0_mirror = Kokkos::create_mirror(X0);
    Kokkos::deep_copy(X0_mirror, X0_host);
    Kokkos::deep_copy(X0, X0_mirror);

    const auto base_node_u = Kokkos::View<double[7]>("base_node_u");
    constexpr auto base_node_u_host_data = std::array{18., 19., 20., 21., 22., 23., 24.};
    const auto base_node_u_host =
        Kokkos::View<double[7], Kokkos::HostSpace>::const_type(base_node_u_host_data.data());
    const auto base_node_u_mirror = Kokkos::create_mirror(base_node_u);
    Kokkos::deep_copy(base_node_u_mirror, base_node_u_host);
    Kokkos::deep_copy(base_node_u, base_node_u_mirror);

    const auto target_node_u = Kokkos::View<double[7]>("target_node_u");
    constexpr auto target_node_u_host_data = std::array{11., 12., 13., 14., 15., 16., 17.};
    const auto target_node_u_host =
        Kokkos::View<double[7], Kokkos::HostSpace>::const_type(target_node_u_host_data.data());
    const auto target_node_u_mirror = Kokkos::create_mirror(target_node_u);
    Kokkos::deep_copy(target_node_u_mirror, target_node_u_host);
    Kokkos::deep_copy(target_node_u, target_node_u_mirror);

    const auto residual_terms = Kokkos::View<double[6]>("residual_terms");
    const auto base_gradient_terms = Kokkos::View<double[6][6]>("base_gradient_terms");
    const auto target_gradient_terms = Kokkos::View<double[6][6]>("target_gradient_terms");

    Kokkos::parallel_for(
        "CalculatePrescribedBCConstraint", 1,
        ExecuteCalculateRigidJointConstraint{
            X0, base_node_u, target_node_u, residual_terms, base_gradient_terms,
            target_gradient_terms
        }
    );

    const auto residual_terms_mirror = Kokkos::create_mirror(residual_terms);
    Kokkos::deep_copy(residual_terms_mirror, residual_terms);

    constexpr auto residual_terms_exact_data = std::array{
        -5900., -2385., -4162., 19.310344827586249, -6.5558669604115494e-14, 38.620689655172455
    };
    const auto residual_terms_exact =
        Kokkos::View<double[6], Kokkos::HostSpace>::const_type(residual_terms_exact_data.data());

    for (auto i = 0U; i < 6U; ++i) {
        EXPECT_NEAR(residual_terms_mirror(i), residual_terms_exact(i), 1.e-12);
    }

    const auto base_gradient_terms_mirror = Kokkos::create_mirror(base_gradient_terms);
    Kokkos::deep_copy(base_gradient_terms_mirror, base_gradient_terms);

    // clang-format off
    constexpr auto base_gradient_terms_exact_data =
        std::array{
            -1., 0., 0., 0., -4158., 2380.,  // Row 1
            0., -1., 0., 4158., 0., -5894.,  // Row 2
            0., 0., -1., -2380., 5894., 0.,  // Row 3
            0., 0., 0., -965.42068965517251, -19.310344827586228, 0.1931034482758299,  // Row 4
            0., 0., 0., 19.310344827586228, -965.51724137931046, -9.6551724137931245,  // Row 5
            0., 0., 0., 0.19310344827589546, 9.6551724137931245, -965.13103448275876   // Row 6
        };
    // clang-format on
    const auto base_gradient_terms_exact = Kokkos::View<double[6][6], Kokkos::HostSpace>::const_type(
        base_gradient_terms_exact_data.data()
    );

    for (auto i = 0U; i < 6U; ++i) {
        for (auto j = 0U; j < 6U; ++j) {
            EXPECT_NEAR(base_gradient_terms_mirror(i, j), base_gradient_terms_exact(i, j), 1.e-12);
        }
    }

    // clang-format off
        constexpr auto target_gradient_terms_exact_data =
            std::array{
                1., 0., 0., 0., 0., 0.,  // Row 1
                0., 1., 0., 0., 0., 0.,  // Row 2
                0., 0., 1., 0., 0., 0.,  // Row 3
                0., 0., 0., 965.42068965517251, -19.310344827586228, -0.19310344827589564,  // Row 4
                0., 0., 0., 19.310344827586228, 965.51724137931046, -9.6551724137931245,    // Row 5
                0., 0., 0., -0.1931034482758299, 9.6551724137931245, 965.13103448275876     // Row 6
            };
    // clang-format on
    const auto target_gradient_terms_exact =
        Kokkos::View<double[6][6], Kokkos::HostSpace>::const_type(
            target_gradient_terms_exact_data.data()
        );

    const auto target_gradient_terms_mirror = Kokkos::create_mirror(target_gradient_terms);
    Kokkos::deep_copy(target_gradient_terms_mirror, target_gradient_terms);

    for (auto i = 0U; i < 6U; ++i) {
        for (auto j = 0U; j < 6U; ++j) {
            EXPECT_NEAR(
                target_gradient_terms_mirror(i, j), target_gradient_terms_exact(i, j), 1.e-12
            );
        }
    }
}

}  // namespace openturbine::tests
