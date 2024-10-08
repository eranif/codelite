cmake_minimum_required(VERSION 3.18)

# Check submodules are here
find_path(CTAGS_SUBMODULE CMakeLists.txt PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ctags/)
if(NOT CTAGS_SUBMODULE)
	message(STATUS "Could not locate `${CMAKE_CURRENT_SOURCE_DIR}/ctags/CMakeLists.txt` file")
	message(FATAL_ERROR "Please make sure you have run `git submodule update --init`")
endif()

# cJSON
add_library(cJSON STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/cJSON/cJSON.c
	${CMAKE_CURRENT_SOURCE_DIR}/cJSON/cJSON.h
)
target_include_directories(cJSON PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cJSON)
