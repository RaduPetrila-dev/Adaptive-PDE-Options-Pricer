cmake_minimum_required(VERSION 3.15)
project(AdaptivePDEPricer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    src/Option.cpp
    src/PDESolver.cpp
    src/Grid.cpp
    src/AdaptiveGrid.cpp
    src/BlackScholes.cpp
)

add_library(pde_pricer_lib ${SOURCES})
add_executable(pde_pricer src/main.cpp)
target_link_libraries(pde_pricer pde_pricer_lib)

enable_testing()
add_subdirectory(tests)
