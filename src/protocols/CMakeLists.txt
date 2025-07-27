# src/protocols/CMakeLists.txt

file(GLOB PROTOCOL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(load_balancer_protocols STATIC ${PROTOCOL_SOURCES})

target_include_directories(load_balancer_protocols PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(load_balancer_protocols PRIVATE
    load_balancer_core
    OpenSSL::SSL
    OpenSSL::Crypto
    spdlog::spdlog)
