set(SRC_DIR src)
set(INC_DIR inc)
FILE(GLOB SRC ${SRC_DIR}/*.c ${SRC_DIR}/*.cpp)
FILE(GLOB HEADERS ${INC_DIR}/*.h ${INC_DIR}/*.hpp)

FILE(GLOB LIB_SRCS ${SRC} ${HEADERS})


# Declare the library
add_library(lib STATIC
    ${SRC}
)

target_link_libraries(lib
                      ncurses)

# Specify here the include directories exported
# by this library
target_include_directories(lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/${INC_DIR}
)
