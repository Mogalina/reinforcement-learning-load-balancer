cmake_minimum_required(VERSION 3.16)
project(load_balancer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output dir for binaries.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include dirs.
include_directories(include)

# Subdirectories.
add_subdirectory(src/core)
add_subdirectory(src/protocols)
add_subdirectory(src/rl)
add_subdirectory(src/utils)

# Final executable.
add_executable(load_balancer src/main.cpp)

# Link all components.
target_link_libraries(load_balancer PRIVATE
    core
    protocols
    rl
    utils)

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
target_link_libraries(protocols PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(utils PRIVATE OpenSSL::SSL OpenSSL::Crypto)
