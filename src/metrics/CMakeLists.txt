# src/metrics/CMakeLists.txt

file(GLOB METRICS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(load_balancer_metrics STATIC ${METRICS_SOURCES})

target_include_directories(load_balancer_metrics PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(load_balancer_metrics PRIVATE
    spdlog::spdlog
    prometheus-cpp::core
    prometheus-cpp::pull)
