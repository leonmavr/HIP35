file(GLOB SRCS *.cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(testfoo ${SRCS})

target_link_libraries(testfoo
    lib
)
