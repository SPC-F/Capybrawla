cmake_minimum_required(VERSION 3.25)
project(Capybrawla)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# =========================================================
# Capycore Engine
# =========================================================
# Freetype configuration (override defaults)
set(FREETYPE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../freetype-win-build")
set(FREETYPE_INCLUDE_DIR "${FREETYPE_ROOT}/include/freetype2")
set(FREETYPE_LIBRARY "${FREETYPE_ROOT}/lib/freetype.lib")
set(FREETYPE_DLL "${FREETYPE_ROOT}/bin/freetype.dll")

FetchContent_Declare(
    CapycoreEngine
    GIT_REPOSITORY https://github.com/SPC-F/CapycoreEngine.git
    GIT_TAG development
)
FetchContent_MakeAvailable(CapycoreEngine)

# =========================================================
# Game Target
# =========================================================
file(GLOB_RECURSE GAME_SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${GAME_SOURCES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        engine
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        enet
        box2d
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${enet_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

# =========================================================
# Copy .dll
# =========================================================
if(WIN32)
    set(CAPYENGINE_BUILD_DIR "${CMAKE_BINARY_DIR}/_deps/capycoreengine-build")
    
    file(GLOB CAPY_DLLS "${CAPYENGINE_BUILD_DIR}/*.dll")
    foreach(dll IN LISTS CAPY_DLLS)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${dll}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endforeach()
endif()
