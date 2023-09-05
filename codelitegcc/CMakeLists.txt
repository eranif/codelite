# Our project is called 'codelite' this is how it will be called in visual studio, and in our makefiles.
project(codelite-cc)

file(GLOB SRCS "*.cpp")

# Define the output
add_executable(codelite-cc ${SRCS})
target_link_libraries(codelite-cc ${LINKER_OPTIONS} tinyjson)

cl_install_executable(codelite-cc)
