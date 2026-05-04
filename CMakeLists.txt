cmake_minimum_required(VERSION 3.12)
project(MACARONI) 
set(CMAKE_CXX_STANDARD 23)


file(GLOB_RECURSE sources CONFIGURE_DEPENDS
     ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(mac ${sources})

target_include_directories(mac PUBLIC ${PROJECT_SOURCE_DIR}/src)

IF(WIN32)
	target_link_directories(mac PUBLIC ${PROJECT_SOURCE_DIR})
ENDIF()

target_link_libraries(mac PUBLIC SDL3)
target_compile_options(mac PUBLIC -g)

option(MACARONI_DEBUG_QUICK_STAGE "" OFF)
if(MACARONI_DEBUG_QUICK_STAGE)
  target_compile_definitions(mac PRIVATE MACARONI_DEBUG_QUICK_STAGE=1)
endif()
