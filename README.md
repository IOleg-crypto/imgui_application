
# ImGui Desktop Application

This repository provides a basic application framework using Dear ImGui, a powerful and easy-to-use graphical user interface (GUI) library for C++.

## 🚀 Features

- Simple and clean ImGui setup for rendering GUIs
- Cross-platform support (works on Windows, Linux, and macOS)
- Easy-to-extend application framework to build on
- Basic examples for quick integration

## 📦 Prerequisites

To build and run this project, ensure the following tools are installed:

- A C++ compiler (e.g., GCC, Clang, LLVM)
- Git for version control
- Visual Studio Code (VSCode) with the necessary extensions for C++ development (C++ extension by Microsoft)
- ImGui library (included as a submodule)

## ⚡ Getting Started

Follow the instructions below to set up and run the project locally.

### 1. Clone the Repository

Start by cloning this repository to your local machine:

```bash
git clone --recursive https://github.com/IOleg-crypto/imgui_application.git
```

The `--recursive` flag ensures that the submodules (such as ImGui) are correctly initialized.

### 2. Open the Project in Visual Studio Code

1. Launch Visual Studio Code (VSCode).
2. Open the folder containing the project by selecting **File -> Open Folder** and choosing the repository directory.
3. Install the necessary VSCode extensions if prompted:
   - **C++ extension by Microsoft** (for C++ IntelliSense, debugging, etc.)
   - **LLVM** ([Download LLVM](https://github.com/llvm/llvm-project))
   - **CMake Tools** (optional, if you plan to build with CMake)

### 3. Configure and Build the Project

1. **Install dependencies**: Make sure you have the necessary build tools and libraries installed. You can do this by following these steps:
   - **Windows**: Install Visual Studio 2019/2022 or use clang/gcc via VSCode.
   - **Linux/macOS**: Install the required dependencies with your package manager (e.g., `sudo apt-get install build-essential` for Ubuntu).

2. **Configure the project in VSCode**:
   - Open the `tasks.json` file in `.vscode` folder (if not created, VSCode will generate it automatically when you try to build the project). The tasks should already be configured for building with `clang-cl.exe` or other compilers.
   
   - Ensure the include paths in your `tasks.json` file are correctly set to the directories where ImGui and your dependencies are located.

3. **Build the project**:
   - In VSCode, press `Ctrl+Shift+B` to build the project using the defined build task.

### 4. Run the Application

Once the project has been successfully built, you can run the application:

- **Windows**: Run `main.exe` from the build directory.
- **Linux/macOS**: Run the output binary from the build folder.

To run the executable from within VSCode, you can use the integrated terminal.

## 🛠 Troubleshooting

### Common Issues:

- **Missing files on Visual Studio**: If you face issues with missing files in Visual Studio 2019/2022, make sure all the necessary ImGui files are included. Add the ImGui source files under `MyAppImGUI/vendor/imgui`.
  
- **Build errors**: Ensure you have the correct dependencies installed, and that your compiler path is set up properly.

## How to include ImGui in Visual Studio Code:

1. Make sure to include the correct ImGui source files under your project settings in VSCode.
2. Add the following directories to the include paths in `tasks.json`:
 
```json
{
	"version": "2.0.0",
	"tasks": [
	  {
	    "type": "cppbuild",
	    "label": "C/C++: clang-cl.exe build active file",
	    //Don`t forget change path to LLVM
	    "command": "E:\\LLVM\\bin\\clang-cl.exe",
	    "args": [
		"/std:c++17",
		"/EHsc",
		"-v",  // verbose output
		"/Zi", // debugging info
		"/Fe${fileDirname}\\${fileBasenameNoExtension}.exe",
		"${file}",
		"${workspaceFolder}\\MyAppImGUI\\vendor\\imgui\\backends\\imgui_impl_win32.cpp",
		"${workspaceFolder}\\MyAppImGUI\\vendor\\imgui\\backends\\imgui_impl_dx11.cpp",
		"${workspaceFolder}\\MyAppImGUI\\vendor\\imgui\\imgui_draw.cpp",
		"${workspaceFolder}\\MyAppImGUI\\vendor\\imgui\\imgui_widgets.cpp",
		"${workspaceFolder}\\MyAppImGUI\\vendor\\imgui\\imgui.cpp",
		"${workspaceFolder}\\MyAppImGUI\\vendor\\imgui\\imgui_tables.cpp",
		"/I", "${workspaceFolder}/MyAppImGUI/vendor/imgui",
		"/I", "${workspaceFolder}/MyAppImGUI/vendor/imgui/backends",
		"/I", "${workspaceFolder}/MyAppImGUI/vendor/imgui/misc",
		//Don`t forget change path to Windows SDK
		"/I", "C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0",
		//Linker add static libs
		"/link",
		"d3d11.lib",
		"dwmapi.lib",
		"gdi32.lib",
		"user32.lib",
		"d3dcompiler.lib",
		"ole32.lib",
		"uuid.lib",
		"oleaut32.lib"
	    ],
	    "options": {
		"cwd": "${fileDirname}"
	    },
	    "problemMatcher": ["$msCompile"],
	    "group": {
		"kind": "build",
		"isDefault": true
	    },
	    "detail": "compiler: clang-cl.exe",
	    "postBuild": {
		"command": "${fileDirname}\\${fileBasenameNoExtension}.exe",
		 "args": [],
	    }

	  }
	]
    }
    
```

You can refer to the official ImGui documentation ([ImGui GitHub](https://github.com/ocornut/imgui)) for more setup details.

## 📷 Screenshots

Sample GUI in Action:
![Sample GUI](image.png)

Another Example:
![Example GUI](image-1.png)

## 📹 Video Tutorial

For a step-by-step guide on setting up and using this project, check out the tutorial playlist

[![IMAGE ALT TEXT HERE](hqdefault.jpg)](https://www.youtube.com/playlist?list=PLTykcSMzD4j8O48ZtfciC9ShZnEvvyNvI)
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/SP6Djf6ku1E/0.jpg)](https://www.youtube.com/watch?v=SP6Djf6ku1E)


Feel free to make any additional changes as needed!
=======


