cmake_minimum_required(VERSION 3.16)

project(hammurabi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hammurabi main.cpp)

include(GNUInstallDirs)
install(TARGETS hammurabi
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
