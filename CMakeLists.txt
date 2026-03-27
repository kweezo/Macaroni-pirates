cmake_minimum_required(VERSION 3.10)
project(MACARONI) 
set(CMAKE_CXX_STANDARD 23)


file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(mac ${sources})

target_include_directories(mac PUBLIC ${PROJECT_SOURCE_DIR}/src)

IF(WIN32)
	target_link_directories(mac PUBLIC ${PROJECT_SOURCE_DIR})
ENDIF()

target_link_libraries(mac PUBLIC SDL3)
target_compile_options(mac PUBLIC -g)
