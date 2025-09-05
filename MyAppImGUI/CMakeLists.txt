cmake_minimum_required(VERSION 3.29)

project(MyAppImGUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(MyAppImGUI
        src/DirectX/d3d_context.cpp
        src/DirectX/d3d_context.h
        src/DirectX/Render.h
        src/DirectX/Render.cpp
        src/FileDialog/FileDialog.cpp
        src/FileDialog/FileDialog.h        
        src/Memory/Memory.h
		src/Memory/Memory.cpp
        src/App/Application.h
		src/App/Application.cpp
        src/Editor/Editor.h
        src/Editor/Editor.cpp
        src/Editor/UIState.h
        vendor/imgui/imgui.h
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/backends/imgui_impl_dx11.cpp
        vendor/imgui/backends/imgui_impl_win32.cpp
        vendor/imgui/misc/cpp/imgui_stdlib.h
        vendor/imgui/misc/cpp/imgui_stdlib.cpp
        src/Window/Window.cpp
        src/Window/Window.h
        src/main.cpp
)

target_include_directories(MyAppImGUI PUBLIC
        src
        src/DirectX
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
)

target_link_libraries(MyAppImGUI PUBLIC
        d3d11
        dxgi
        dxguid
        ole32
        shell32
        user32
)

if (MSVC)
    target_link_options(MyAppImGUI PRIVATE
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
        $<$<CONFIG:Release>:/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS>
    )
endif()
