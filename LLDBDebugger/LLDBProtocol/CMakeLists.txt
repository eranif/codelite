project(LLDBProtocol)

FILE(GLOB SRC "*.cpp")

add_library(LLDBProtocol STATIC ${SRC})
