cmake_minimum_required(VERSION 3.29)
project(MyAppImGUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

add_executable(MyAppImGUI
    src/FileDialog/FileDialog.cpp
    src/FileDialog/FileDialog.h
    src/Memory/Memory.cpp
    src/Memory/Memory.h
    src/App/Application.cpp
    src/App/Application.h
    src/Editor/Editor.cpp
    src/Editor/Editor.h
    src/Editor/UIState.h
    src/Window/Window.cpp
    src/Window/Window.h
    src/main.cpp
)

include(FetchContent)

# Native File Dialog
FetchContent_Declare(
  nfd
  GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
  GIT_TAG        v1.2.1
)
FetchContent_MakeAvailable(nfd)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.9
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# stb
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(stb)
include_directories(${stb_SOURCE_DIR})

# ImGui core
add_library(imguilib STATIC
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui.h
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(imguilib PUBLIC
    vendor/imgui
    vendor/imgui/misc/cpp
)

# ImGui backends
add_library(opengl3imgui STATIC
    vendor/imgui/backends/imgui_impl_opengl3.cpp
    vendor/imgui/backends/imgui_impl_opengl3.h
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_glfw.h
)
target_include_directories(opengl3imgui PUBLIC
    vendor/imgui
    vendor/imgui/backends
)
find_package(OpenGL REQUIRED)
target_link_libraries(opengl3imgui PUBLIC imguilib glfw)

target_include_directories(MyAppImGUI PUBLIC src)

if(UNIX)
    target_link_libraries(MyAppImGUI PUBLIC
        opengl3imgui
        imguilib
        glfw
        nfd
        GL
        X11
        pthread
        dl
    )
elseif(WIN32)
    target_link_libraries(MyAppImGUI PUBLIC
        opengl3imgui
        imguilib
        glfw
        nfd
        user32 gdi32 shell32 ole32 uuid comdlg32 opengl32
    )
endif()

if (MSVC)
    target_link_options(MyAppImGUI PRIVATE
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
    )
endif()
