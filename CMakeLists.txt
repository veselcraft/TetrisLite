cmake_minimum_required(VERSION 3.0)

project(TetrisLite)

add_executable(${PROJECT_NAME} main.cpp menu.cpp)

find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2 REQUIRED sdl2)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
)

if(PSP)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2main
        SDL2_mixer
        SDL2_image
        SDL2_ttf
        vorbisfile
        vorbis
        ogg
        jpeg
        png
        z
        freetype
    )
    create_pbp_file(
        TARGET ${PROJECT_NAME}
        ICON_PATH ICON0.png
        BACKGROUND_PATH NULL
        PREVIEW_PATH NULL
        TITLE ${PROJECT_NAME}
    )
endif()
