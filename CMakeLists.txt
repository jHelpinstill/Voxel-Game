cmake_minimum_required(VERSION 3.20)

project(voxel_game)
set(target_name voxel_game)

# Dynamically find all source files in the src directory
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "source/*.cpp" "source/*.c")

add_executable(${target_name} ${SOURCES})

# include_directories(include dependencies/include)
target_include_directories(${target_name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/include
    C:/repos/freetype/include
)

target_link_libraries(${target_name} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/lib/libglfw3.a
    "C:/repos/freetype/build/libfreetyped.a"
)
# include(FetchContent)

# FetchContent_Declare(
#     glfw
#     GIT_REPOSITORY https://github.com/glfw/glfw 
#     GIT_TAG "master"
#     GIT_SHALLOW 1
# )

# FetchContent_MakeAvailable(glfw)

# FetchContent_Declare(
#     glad
#     GIT_REPOSITORY https://github.com/Sunday111/glad 
#     GIT_TAG "main"
#     GIT_SHALLOW 1
# )

# FetchContent_MakeAvailable(glad)

