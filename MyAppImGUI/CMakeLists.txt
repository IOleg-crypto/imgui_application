cmake_minimum_required(VERSION 3.29)

project(MyAppImGUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(MyAppImGUI
        src/OpenGL/Render.h
        src/OpenGL/Render.cpp
        src/FileDialog/FileDialog.cpp
        src/FileDialog/FileDialog.h        
        src/Memory/Memory.h
		src/Memory/Memory.cpp
        src/App/Application.h
		src/App/Application.cpp
        src/Editor/Editor.h
        src/Editor/Editor.cpp
        src/Editor/UIState.h        
        src/Window/Window.cpp
        src/Window/Window.h
        src/main.cpp
)

add_library(imgui STATIC 
        vendor/imgui/imgui.h
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/misc/cpp/imgui_stdlib.h
        vendor/imgui/misc/cpp/imgui_stdlib.cpp)

add_library(opengl3imgui STATIC 
       vendor/imgui/backends/imgui_impl_opengl3.cpp
       vendor/imgui/backends/imgui_impl_opengl3_loader.h
       vendor/imgui/backends/imgui_impl_glfw.h
       vendor/imgui/backends/imgui_impl_glfw.cpp
       )

target_include_directories(MyAppImGUI PUBLIC
        src
        src/OpenGL
        src/FileDialog
        src/Memory
        vendor/imgui
        vendor/imgui/backends
        vendor/imgui/examples/example_allegro5
        vendor/imgui/examples/libs/emscripten
        vendor/imgui/examples/libs/glfw
        vendor/imgui/misc/cpp
        vendor/imgui/misc/freetype
        vendor/imgui/misc/single_file
        vendor/imgui/examples/libs/glfw/lib-vc2010-64

)

target_link_libraries(MyAppImGUI PUBLIC
        opengl3imgui
        imgui
        glfw
        opengl32.lib
        d3d11.lib
        dxgi.lib
        winmm.lib
        user32.lib
        gdi32.lib
        shell32.lib
        ole32.lib
        uuid.lib
        comdlg32.lib
)

if (MSVC)
    target_link_options(MyAppImGUI PRIVATE
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
        $<$<CONFIG:Release>:/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS>
    )
endif()
