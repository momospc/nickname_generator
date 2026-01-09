cmake_minimum_required(VERSION 3.16)
project(nickname_generator_sdl2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(nickname_generator
    NICKNAME-generator_SDL2.cpp
)

target_link_libraries(nickname_generator PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_ttf::SDL2_ttf
)

if (WIN32)
    set_target_properties(nickname_generator PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

if (EXISTS "${CMAKE_SOURCE_DIR}/Roboto-Regular.ttf")
    add_custom_command(TARGET nickname_generator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/Roboto-Regular.ttf
        $<TARGET_FILE_DIR:nickname_generator>
    )
endif()

