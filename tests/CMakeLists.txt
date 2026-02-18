include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(pde_tests
    test_european.cpp
    test_american.cpp
    test_grid.cpp
    test_edge_cases.cpp
)

target_link_libraries(pde_tests PRIVATE pde_pricer_lib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(pde_tests)
