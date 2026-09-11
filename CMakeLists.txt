cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_GENERATOR "Ninja")
set(CMAKE_BUILD_TYPE "Debug")

project(BulletHand)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/raylib EXCLUDE_FROM_ALL)
add_subdirectory(external/box2d EXCLUDE_FROM_ALL)

add_executable(game)

target_sources(game
  PRIVATE
    main/main.cpp

  PUBLIC
    FILE_SET headers
    TYPE HEADERS
    BASE_DIRS
      external/raylib/src
      external/raygui
      external/box2d/include
      main/include
    FILES
      external/raylib/src/raylib.h
      external/raygui/raygui.h
      external/box2d/include/box2d/box2d.h
)

target_link_libraries(game
  PRIVATE
    raylib
    box2d
    GL
    m
    pthread
    dl
    rt
    X11
)

# add_custom_command(
#   TARGET game POST_BUILD
#   COMMAND
#     ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets
#     $<TARGET_FILE_DIR:game>/assets
# )
