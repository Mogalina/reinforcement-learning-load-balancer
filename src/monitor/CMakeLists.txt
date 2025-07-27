# src/monitor/CMakeLists.txt

file(GLOB MONITOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(load_balancer_monitor STATIC ${MONITOR_SOURCES})

target_include_directories(load_balancer_monitor PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(load_balancer_monitor PUBLIC spdlog::spdlog)
