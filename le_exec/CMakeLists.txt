project(codelite-exec)

# prepare list of files
FILE(GLOB SRCS "*.cpp")
add_executable(codelite-exec ${SRCS})
