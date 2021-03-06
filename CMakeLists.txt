cmake_minimum_required (VERSION 3.7)
cmake_policy(VERSION 3.7...3.20)
option(USE_GTEST "USE_GTEST" OFF)

project (TestVulkanPayloadArray)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
include_directories(src)

set(DATA_PATH "${CMAKE_SOURCE_DIR}/Data" CACHE PATH "Location of folder 'Data'")
add_definitions(-DDATA_PATH=\"${DATA_PATH}\")

if(WIN32)
	add_executable(TestVulkanPayloadArray WIN32 ${SOURCES})
else()
	add_executable(TestVulkanPayloadArray ${SOURCES})
endif()

# Hide console window in release build on Windows/MSVC.
# See: https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/recipe/VSConfigSpecificSettings
if(MSVC)
	set_target_properties(TestVulkanPayloadArray PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(TestVulkanPayloadArray PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
	set_target_properties(TestVulkanPayloadArray PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(TestVulkanPayloadArray PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(TestVulkanPayloadArray PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set_target_properties(TestVulkanPayloadArray PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif()

if(MSVC)
    target_compile_options(TestVulkanPayloadArray PRIVATE "/MP")
endif()

set(FPHSA_NAME_MISMATCHED TRUE)
if(VCPKG_TOOLCHAIN)
	find_package(SDL2 CONFIG REQUIRED)
else()
	find_package(SDL2 REQUIRED)
endif()
if((${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles") OR (${CMAKE_GENERATOR} STREQUAL "MSYS Makefiles"))
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
	target_link_libraries(TestVulkanPayloadArray PRIVATE mingw32)
endif()
if(VCPKG_TOOLCHAIN)
	target_link_libraries(TestVulkanPayloadArray PRIVATE SDL2::SDL2main)
else()
	target_link_libraries(TestVulkanPayloadArray PRIVATE SDL2::Main)
endif()

cmake_policy(SET CMP0012 NEW)
find_package(sgl REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(Vulkan REQUIRED)
target_link_libraries(TestVulkanPayloadArray PUBLIC sgl ${Boost_LIBRARIES})
include_directories(${sgl_INCLUDES} ${Boost_INCLUDE_DIR})

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.21.0")
    target_link_libraries(TestVulkanPayloadArray PRIVATE Vulkan::Headers)
endif()

find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
