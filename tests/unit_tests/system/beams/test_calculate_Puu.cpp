#include <Kokkos_Core.hpp>
#include <gtest/gtest.h>

#include "system/beams/calculate_Puu.hpp"
#include "test_calculate.hpp"

namespace {

struct TestFunctionObject {
    Kokkos::View<double[6][6]>::const_type Cuu;
    Kokkos::View<double[3][3]>::const_type x0pupSS;
    Kokkos::View<double[3][3]>::const_type N_tilde;
    Kokkos::View<double[6][6]> Puu;

    KOKKOS_FUNCTION
    void operator()(size_t) const { openturbine::beams::CalculatePuu(Cuu, x0pupSS, N_tilde, Puu); }
};

void TestCalculatePuu() {
    const auto Cuu = Kokkos::View<double[6][6]>("Cuu");
    constexpr auto Cuu_data = std::array{1.,  2.,  3.,  4.,  5.,  6.,  7.,  8.,  9.,  10., 11., 12.,
                                         13., 14., 15., 16., 17., 18., 19., 20., 21., 22., 23., 24.,
                                         25., 26., 27., 28., 29., 30., 31., 32., 33., 34., 35., 36.};
    const auto Cuu_host = Kokkos::View<double[6][6], Kokkos::HostSpace>::const_type(Cuu_data.data());
    const auto Cuu_mirror = Kokkos::create_mirror(Cuu);
    Kokkos::deep_copy(Cuu_mirror, Cuu_host);
    Kokkos::deep_copy(Cuu, Cuu_mirror);

    const auto x0pupSS = Kokkos::View<double[3][3]>("x0pupSS");
    constexpr auto x0pupSS_data = std::array{37., 38., 39., 40., 41., 42., 43., 44., 45.};
    const auto x0pupSS_host =
        Kokkos::View<double[3][3], Kokkos::HostSpace>::const_type(x0pupSS_data.data());
    const auto x0pupSS_mirror = Kokkos::create_mirror(x0pupSS);
    Kokkos::deep_copy(x0pupSS_mirror, x0pupSS_host);
    Kokkos::deep_copy(x0pupSS, x0pupSS_mirror);

    const auto N_tilde = Kokkos::View<double[3][3]>("N_tilde");
    constexpr auto N_tilde_data = std::array{55., 56., 57., 58., 59., 60., 61., 62., 63.};
    const auto N_tilde_host =
        Kokkos::View<double[3][3], Kokkos::HostSpace>::const_type(N_tilde_data.data());
    const auto N_tilde_mirror = Kokkos::create_mirror(N_tilde);
    Kokkos::deep_copy(N_tilde_mirror, N_tilde_host);
    Kokkos::deep_copy(N_tilde, N_tilde_mirror);

    const auto Puu = Kokkos::View<double[6][6]>("Puu");

    Kokkos::parallel_for("CalculatePuu", 1, TestFunctionObject{Cuu, x0pupSS, N_tilde, Puu});

    constexpr auto Puu_exact_data =
        std::array{0.,   0.,    0.,    0.,    0.,    0.,    0.,   0.,    0.,    0.,    0.,    0.,
                   0.,   0.,    0.,    0.,    0.,    0.,    931., 1052., 1173., 1236., 1356., 1476.,
                   955., 1079., 1203., 1266., 1389., 1512., 979., 1106., 1233., 1296., 1422., 1548.};
    const auto Puu_exact =
        Kokkos::View<double[6][6], Kokkos::HostSpace>::const_type(Puu_exact_data.data());

    const auto Puu_mirror = Kokkos::create_mirror(Puu);
    Kokkos::deep_copy(Puu_mirror, Puu);
    openturbine::tests::CompareWithExpected(Puu_mirror, Puu_exact);
}

}  // namespace

namespace openturbine::tests {

TEST(CalculatePuuTests, OneNode) {
    TestCalculatePuu();
}

}  // namespace openturbine::tests
