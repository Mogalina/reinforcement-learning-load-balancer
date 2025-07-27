# src/utils/CMakeLists.txt

file(GLOB UTILS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(load_balancer_utils STATIC ${UTILS_SOURCES})

target_include_directories(load_balancer_utils PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(load_balancer_utils PRIVATE
    spdlog::spdlog
    OpenSSL::SSL
    OpenSSL::Crypto)
