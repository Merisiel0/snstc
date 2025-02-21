cmake_minimum_required(VERSION 3.10)

project(Sphere)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Sphere ${SOURCES})

find_package(EnTT CONFIG REQUIRED)
target_link_libraries(Sphere PRIVATE EnTT::EnTT)

find_package(glm CONFIG REQUIRED)
target_link_libraries(Sphere PRIVATE glm::glm)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(Sphere PRIVATE SDL3::SDL3)

find_package(Vulkan REQUIRED)
target_link_libraries(Sphere PRIVATE Vulkan::Vulkan)

find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(Sphere PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)

target_include_directories(Sphere PRIVATE ${CMAKE_SOURCE_DIR}/build/vcpkg_installed/installed/x64-linux/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif()

if(WIN32)
    set(VCPKG_MANIFEST_FEATURES "vulkan")
else()
    set(VCPKG_MANIFEST_FEATURES "vulkan;x11")
endif()