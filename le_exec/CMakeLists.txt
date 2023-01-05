project(codelite-exec)

# prepare list of files
FILE(GLOB SRCS "*.cpp")
add_executable(codelite-exec ${SRCS})

# install the binary
codelite_install_executable(codelite-exec)
