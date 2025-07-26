# src/rl/CMakeLists.txt

file(GLOB RL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(rl STATIC ${RL_SOURCES})

target_include_directories(rl PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
