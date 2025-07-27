# src/rl/CMakeLists.txt

file(GLOB RL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(load_balancer_rl STATIC ${RL_SOURCES})

target_include_directories(load_balancer_rl PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
