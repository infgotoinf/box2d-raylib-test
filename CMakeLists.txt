cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_GENERATOR "Ninja")

project(BulletHand)

add_executable(game)

target_sources(game
  PRIVATE
    main/main.cpp

  PUBLIC
    FILE_SET headers
    TYPE HEADERS
    BASE_DIRS
      external/raylib
      external/raygui
      external/box2d/include
      main/include
    FILES
      external/raylib/raylib.h
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
