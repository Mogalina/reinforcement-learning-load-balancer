file(GLOB UTILS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(utils STATIC ${UTILS_SOURCES})

target_include_directories(utils PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(utils PUBLIC spdlog::spdlog)
