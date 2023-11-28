file(GLOB SRCS *.cpp *.c)

set(CMAKE_CXX_STANDARD 17)

add_executable(testhip35 ${SRCS})

target_link_libraries(testhip35
    m # glib math library (-lm)
    hip35 # library built by this project
)
