file(GLOB SRCS *.cpp)

set(CMAKE_CXX_STANDARD 11)

add_executable(testfoo ${SRCS})

target_link_libraries(testfoo
    lib
)
