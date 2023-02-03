project(makedir)
include(${wxWidgets_USE_FILE})

# prepare list of files
file(GLOB SRCS "*.cpp")
add_executable(makedir ${SRCS})
target_link_libraries(makedir ${wxWidgets_LIBRARIES})
