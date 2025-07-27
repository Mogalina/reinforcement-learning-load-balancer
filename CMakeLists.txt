cmake_minimum_required(VERSION 3.16)
project(load_balancer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output dir for binaries.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories.
include_directories(include)

# Enable logging.
Include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.9.2
)
FetchContent_MakeAvailable(spdlog)

# Enable TLS/HTTPS support.
find_package(OpenSSL REQUIRED)

# Prometheus client.
FetchContent_Declare(
    prometheus-cpp
    GIT_REPOSITORY https://github.com/jupp0r/prometheus-cpp.git
    GIT_TAG        v1.0.0
)
FetchContent_MakeAvailable(prometheus-cpp)

# Subdirectories.
add_subdirectory(src/core)
add_subdirectory(src/protocols)
add_subdirectory(src/rl)
add_subdirectory(src/utils)
add_subdirectory(src/monitor)
add_subdirectory(src/metrics)

# Final executable.
add_executable(load_balancer src/main.cpp)

# Link all components.
target_link_libraries(load_balancer PRIVATE
    load_balancer_core
    load_balancer_protocols
    load_balancer_rl
    load_balancer_utils
    load_balancer_monitor
    load_balancer_metrics)
