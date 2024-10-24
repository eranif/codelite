cmake_minimum_required(VERSION 3.18)

# ##############################################################################
# Check submodules are here
# ##############################################################################
find_path(CTAGS_SUBMODULE CMakeLists.txt PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ctags/)
if(NOT CTAGS_SUBMODULE)
	message(STATUS "Could not locate `${CMAKE_CURRENT_SOURCE_DIR}/ctags/CMakeLists.txt` file")
	message(FATAL_ERROR "Please make sure you have run `git submodule update --init`")
endif()

# ##############################################################################
# cJSON
# ##############################################################################
add_library(cJSON STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/cJSON/cJSON.c
	${CMAKE_CURRENT_SOURCE_DIR}/cJSON/cJSON.h
)
target_include_directories(cJSON PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cJSON)

# ##############################################################################
# For wxWidgets dependencies
# ##############################################################################
include("${wxWidgets_USE_FILE}")

# ##############################################################################
# wxshapeframework
# ##############################################################################
file(GLOB WXSF_SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/wxsf-code/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/wxsf-code/src/wxxmlserializer/*.cpp"
)
add_library(wxshapeframework SHARED ${WXSF_SRCS})

target_include_directories(wxshapeframework PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/wxsf-code/src")
target_include_directories(wxshapeframework PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/wxsf-code/include")

if(WIN32)
	target_compile_definitions(wxshapeframework PRIVATE WXMAKINGDLL_WXXS WXMAKINGDLL_WXSF)
	target_compile_definitions(wxshapeframework INTERFACE WXUSINGDLL)
endif(WIN32)

if(UNIX OR APPLE)
    set_target_properties(wxshapeframework PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_link_libraries(wxshapeframework PRIVATE ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES})

codelite_install_library_target(wxshapeframework)
