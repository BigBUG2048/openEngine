cmake_minimum_required(VERSION 3.20)
project(openEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(oe_common STATIC
    targets/common/error.cpp
    targets/common/validation.cpp
    targets/common/debug_layer.cpp
    targets/common/device_context.cpp
    targets/common/device.cpp
    targets/common/buffer.cpp
    targets/common/texture.cpp
    targets/common/pipeline.cpp
    targets/common/command_buffer.cpp
    targets/common/sync.cpp
    targets/common/swapchain.cpp
    targets/common/backend_factory.cpp
    targets/null/null_backend.cpp
)

target_include_directories(oe_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/targets/common
        ${CMAKE_CURRENT_SOURCE_DIR}/targets/null
)

add_executable(common_api_tests tests/common_api_test.cpp)
target_link_libraries(common_api_tests PRIVATE oe_common)

enable_testing()
add_test(NAME common_api_tests COMMAND common_api_tests)
