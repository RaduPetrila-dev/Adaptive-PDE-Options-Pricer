cmake_minimum_required(VERSION 3.15)
project(Adaptive-PDE-Options-Pricer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/Option.cpp
    src/PDESolver.cpp
    src/Grid.cpp
    src/AdaptiveGrid.cpp
    src/BlackScholes.cpp
)

# Static library for the pricing engine
add_library(pde_pricer_lib STATIC ${SOURCES})
target_include_directories(pde_pricer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(pde_pricer_lib PRIVATE /O2 /W4)
else()
    target_compile_options(pde_pricer_lib PRIVATE -O3 -Wall -Wextra -Wpedantic)
endif()

# Main executable
add_executable(pde_pricer src/main.cpp)
target_link_libraries(pde_pricer PRIVATE pde_pricer_lib)

# Testing
enable_testing()
add_subdirectory(tests)
