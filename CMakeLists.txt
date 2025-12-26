cmake_minimum_required(VERSION 3.16)

project(hammurabi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SAVE_FILE ${CMAKE_SOURCE_DIR}/hammurabi_save.json)

add_executable(hammurabi 
    main.cpp
    "city_state.cpp"
    "city_events.cpp"
    "rullers_decisions.cpp"
    "round_state.cpp"
    "utils.cpp"
    "city_state.h"
    "city_events.h"
    "rullers_decisions.h"
    "round_state.h"
    "utils.h"
)

# Для отладки с определённым начальным значением для генерации
target_compile_definitions(hammurabi PRIVATE SEED=1)
target_compile_definitions(hammurabi PRIVATE SAVE_FILE="${SAVE_FILE}")

target_include_directories(hammurabi
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/json
)

include(GNUInstallDirs)
install(TARGETS hammurabi
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
