cmake_minimum_required(VERSION 3.0.0)
project(Orange VERSION 0.1.0)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Orange_SOURCE_DIR}/bin)
include(CTest)
enable_testing()
include(cmake/CPM.cmake)

CPMAddPackage("gh:libsdl-org/SDL#release-2.0.22")
CPMAddPackage("gh:cginternals/glbinding#v3.1.0")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")

add_executable(Orange src/main.cpp src/model.cpp)

target_link_libraries(Orange SDL2::SDL2 SDL2::SDL2main glbinding::glbinding glm::glm)

set_property(TARGET Orange PROPERTY CXX_STANDARD 20)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
