# src/metrics/CMakeLists.txt

file(GLOB METRICS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(metrics STATIC ${METRICS_SOURCES})

target_include_directories(metrics PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(metrics PUBLIC spdlog::spdlog)
