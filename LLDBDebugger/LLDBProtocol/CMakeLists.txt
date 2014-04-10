project(LLDBProtocol)

FILE(GLOB SRC "*.cpp")
add_definitions(-fPIC)
add_library(LLDBProtocol STATIC ${SRC})
