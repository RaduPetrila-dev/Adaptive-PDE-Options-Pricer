cmake_minimum_required(VERSION 3.15)
project(Adaptive-PDE-Options-Pricer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files
set(SOURCES
    src/Option.cpp
    src/PDESolver.cpp
    src/Grid.cpp
    src/AdaptiveGrid.cpp
    src/BlackScholes.cpp
)

# Create the library (Explicitly STATIC or SHARED is better, usually STATIC for internal logic)
add_library(pde_pricer_lib STATIC ${SOURCES})

# Modern Include Management:
# PUBLIC means: 
# 1. The library itself can use headers in 'include/'
# 2. Anyone linking to this library (like your main.cpp) can ALSO see 'include/'
target_include_directories(pde_pricer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Modern Compiler Options:
# Applies flags only to this library. 
# CAUTION: Keep -march=native only if you don't plan to distribute the binary to other machines.
if(MSVC)
    target_compile_options(pde_pricer_lib PRIVATE /O2)
else()
    target_compile_options(pde_pricer_lib PRIVATE -O3 -march=native)
endif()

# Create the executable
add_executable(pde_pricer src/main.cpp)

# Link the library
# PRIVATE means pde_pricer uses the lib, but doesn't expose it to others (irrelevant for exe, but good habit)
target_link_libraries(pde_pricer PRIVATE pde_pricer_lib)

enable_testing()
add_subdirectory(tests)
