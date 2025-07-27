# src/monitor/CMakeLists.txt

file(GLOB MONITOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(monitor STATIC ${MONITOR_SOURCES})

target_include_directories(monitor PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(monitor PUBLIC spdlog::spdlog)
