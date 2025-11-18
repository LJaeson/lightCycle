cmake_minimum_required(VERSION 3.16)
project(LightCycle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_BUILD_TYPE Debug)  # Debug mode
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=address")

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(light_cycle
    src/lightCycle/lightCycle.cc
    src/lightCycle/Game.cc
    src/lightCycle/Map.cc
    src/lightCycle/utility.cc
    src/movable/Actor.cc
    src/movable/Player.cc
    src/movable/Bot.cc
    src/movable/Bot2.cc
    src/movable/Bot3.cc
    src/movable/Bot4.cc
    src/lightCycle/GameState.cc
)

target_compile_features(light_cycle PRIVATE cxx_std_17)

target_link_libraries(light_cycle PRIVATE
  SFML::Graphics
  SFML::Window
  SFML::Audio
)

target_include_directories(light_cycle
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Where the exe goes (optional but neat)
set_target_properties(light_cycle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Copy resources next to the exe after build
add_custom_command(
    TARGET light_cycle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:light_cycle>/resources
    COMMENT "Copying resources/"
)

# macOS rpath (Homebrew)
if(APPLE)
    set_target_properties(light_cycle PROPERTIES
        BUILD_RPATH "/usr/local/lib;/opt/homebrew/lib"
        INSTALL_RPATH "/usr/local/lib;/opt/homebrew/lib"
    )
endif()
