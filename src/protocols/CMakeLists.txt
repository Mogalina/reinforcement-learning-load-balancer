file(GLOB PROTOCOL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(protocols STATIC ${PROTOCOL_SOURCES})

target_include_directories(protocols PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(protocols PUBLIC core)
target_link_libraries(protocols PUBLIC spdlog::spdlog)
