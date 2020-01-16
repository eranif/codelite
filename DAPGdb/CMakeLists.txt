cmake_minimum_required(VERSION 2.8)
project(gdbd)
add_subdirectory(dap)

file(GLOB SRCS "*.cpp")
add_executable(gdbd ${SRCS})
target_link_libraries(gdbd dapcxx)

