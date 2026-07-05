cmake_minimum_required(VERSION 3.16)
project(snake_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# raylib is the only dependency that actually needs building, so it's the
# only one fetched as a full CMake project. doctest and nlohmann/json are
# vendored as single headers in third_party/ to keep configure time down.
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# --- snake_core: the simulation, no raylib dependency -----------------------
# Everything a unit test needs to link against, and nothing a unit test
# would need a display or a GPU for.
add_library(snake_core STATIC
    src/snake.cpp
    src/food.cpp
    src/highscore.cpp
    src/replay.cpp
    src/ai/pathfinding.cpp
    src/ai/hamiltonian.cpp
    src/ai/snake_ai.cpp
)
target_include_directories(snake_core PUBLIC src third_party)

# --- snake_cpp: the playable game, links raylib for rendering/audio/input ---
add_executable(snake_cpp
    src/main.cpp
    src/game.cpp
    src/renderer.cpp
    src/audio_manager.cpp
    src/input_handler.cpp
    src/menu.cpp
)
target_link_libraries(snake_cpp PRIVATE snake_core raylib)

add_custom_command(TARGET snake_cpp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:snake_cpp>/assets
    COMMENT "Copying assets next to the built binary"
)

# --- snake_tests: doctest, links only snake_core -----------------------------
enable_testing()
add_executable(snake_tests
    tests/test_main.cpp
    tests/test_snake.cpp
    tests/test_food.cpp
    tests/test_highscore.cpp
    tests/test_pathfinding.cpp
    tests/test_hamiltonian.cpp
    tests/test_snake_ai.cpp
    tests/test_replay.cpp
)
target_include_directories(snake_tests PRIVATE third_party)
target_link_libraries(snake_tests PRIVATE snake_core)
add_test(NAME snake_tests COMMAND snake_tests)
