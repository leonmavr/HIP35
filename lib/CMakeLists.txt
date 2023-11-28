set(SRC_DIR src)
set(INC_DIR inc)
FILE(GLOB SRC ${SRC_DIR}/*.c ${SRC_DIR}/*.cpp)
FILE(GLOB HEADERS ${INC_DIR}/*.h ${INC_DIR}/*.hpp)

FILE(GLOB LIB_SRCS ${SRC} ${HEADERS})

# Declare the library
add_library(hip35 STATIC
    ${SRC}
)

# Enable compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(hip35 PRIVATE -Wall -Wextra -pedantic)
elseif(MSVC)
    target_compile_options(hip35 PRIVATE /W4)
endif()

target_link_libraries(hip35
                      ncurses)

# Specify here the include directories exported
# by this library
target_include_directories(hip35 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/${INC_DIR}
)
