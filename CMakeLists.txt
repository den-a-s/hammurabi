cmake_minimum_required(VERSION 3.16)

project(hammurabi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hammurabi 
    main.cpp
    "city_state.h"
    "city_state.cpp"
    "city_events.h"
    "city_events.cpp"
    "rullers_decisions.h"
    "rullers_decisions.cpp"
    "round_state.h"
    "round_state.cpp"
    "utils.h" 
     )

include(GNUInstallDirs)
install(TARGETS hammurabi
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
