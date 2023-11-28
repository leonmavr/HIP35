set(MAIN_SRCS
    main.cpp
)

add_executable(demo
    ${MAIN_SRCS}
)

# Specify here the libraries this program depends on
target_link_libraries(demo
    hip35 # library built by this project
    ncurses
)

install(TARGETS demo DESTINATION bin)
