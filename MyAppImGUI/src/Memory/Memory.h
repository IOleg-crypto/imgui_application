#ifndef MEMORY_H
#define MEMORY_H

// Enable debug memory allocation tracking in Visual Studio
#define CRTDBG_MAP_ALLOC
#include <memory>     // (Optional) For smart pointers if needed
#include <iostream>   // (Optional) For logging/debug output
#ifdef _WIN32
#include <crtdbg.h>   // For memory leak detection and debugging
#endif
#include "imgui.h"

#if CHECK_MEMORYALLOC
// Static variable to count memory allocations
// Note: 'static' makes this variable local to each translation unit (.cpp file)
// If you want a global counter across all files, use 'extern' instead
static uint32_t s_allocation = 0;

// Custom global 'new' operator
// Allows tracking or overriding memory allocations
void* operator new(const size_t size);

// Custom global 'delete' operator (sized version, available in C++14 and later)
// Allows tracking or overriding memory deallocation
void operator delete(void* ptr, const size_t size) noexcept;
#endif
/*
 * Callback function to handle dynamic buffer resizing for ImGui::InputTextMultiline.
 * This is required when using std::string with ImGui input fields.
 *
 * Parameters:
 * - data: pointer to ImGuiInputTextCallbackData, which contains info about the input buffer and user data
 *
 * Behavior:
 * - If the buffer is empty, frees memory by swapping with an empty string
 * - If the buffer grows, resizes the underlying std::string to accommodate the new text
 * - Updates the buffer pointer to point to the new memory
 */
static int InputTextCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Retrieve our std::string pointer from UserData.
        auto* str = static_cast<std::string*>(data->UserData);

        std::cout << "BufferText : " << data->BufTextLen << "\n";

        if (data->BufTextLen == 0)
        {
            // When the text is empty, free extra memory by swapping with an empty string.
            std::string().swap(*str);
        }
        else if (data->BufTextLen + 1 > static_cast<int>(str->size()))
        {
            // Resize the string to fit the new content (+1 for null terminator)
            str->resize(data->BufTextLen + 1);
        }

        // Let ImGui know where the resized buffer is
        data->Buf = str->data();
    }
    return 0;
}


#endif // MEMORY_H