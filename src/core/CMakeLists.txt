# src/core/CMakeLists.txt

file(GLOB CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(load_balancer_core STATIC ${CORE_SOURCES})

target_include_directories(load_balancer_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(load_balancer_core PRIVATE
    load_balancer_rl
    spdlog::spdlog)
