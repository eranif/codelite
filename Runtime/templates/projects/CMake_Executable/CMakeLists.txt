# Use the variable PROJECT_NAME for changing the target name
set(PROJECT_NAME "HelloWorld")

# Set our project name
project(${PROJECT_NAME})

# Use all the *.cpp files we found under this folder for the project
file(GLOB SRCS "*.cpp" "*.c")

# Define the executable
add_executable(${PROJECT_NAME} ${SRCS})
