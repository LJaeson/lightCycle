cmake_minimum_required(VERSION 3.16)
project(LightCycle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window Audio)

add_executable(light_cycle
  src/lightCycle.cc
  include/lightCycle/lightCycle.hh
)

target_link_libraries(light_cycle PRIVATE
  SFML::Graphics
  SFML::Window
  SFML::Audio
)

target_include_directories(light_cycle
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
