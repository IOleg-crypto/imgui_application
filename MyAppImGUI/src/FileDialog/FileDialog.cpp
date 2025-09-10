#include "FileDialog.h"
#include "nfd.h"

#include <iostream>
#include <fstream>
#include "nfd_glfw3.h"
#include "nfd.h"


void SaveFileDialog(const std::string& CurrentTabInfo, std::string& path)
{
    NFD_Init();

    nfdu8char_t* outPath = nullptr;
    nfdu8filteritem_t filters[] = { { "Text/Binary files", "txt,bin" } };

    nfdresult_t result = NFD_SaveDialogU8(&outPath, filters, 1, nullptr , nullptr);

    if (result == NFD_OKAY)
    {
        std::string path = outPath;
        bool isBinary = (path.find(".bin") != std::string::npos);

        std::ofstream outFile(path, isBinary ? (std::ios::binary | std::ios::trunc) : std::ios::trunc);
        if (outFile)
        {
            if (isBinary)
                outFile.write(CurrentTabInfo.c_str(), CurrentTabInfo.size());
            else
                outFile << CurrentTabInfo;

            outFile.close();
            std::cout << "File saved: " << path << std::endl;
        }
        else
        {
            std::cerr << "Error saving file: " << path << std::endl;
        }

        NFD_FreePathU8(outPath);
    }
    else if (result == NFD_CANCEL)
    {
        std::cout << "User canceled save dialog" << std::endl;
    }
    else
    {
        std::cerr << "NFD error: " << NFD_GetError() << std::endl;
    }

    NFD_Quit();
}

void ShowOpenFileDialog(std::string& tabContents, std::string& pathFile)
{
    NFD_Init();

    nfdu8char_t* outPath = nullptr;
    nfdu8filteritem_t filters[] = { { "Text/Binary files", "txt,bin" } };

    nfdresult_t result = NFD_OpenDialogU8(&outPath, filters, 1, nullptr);

    if (result == NFD_OKAY)
    {
        pathFile = outPath;
        bool isBinary = (pathFile.find(".bin") != std::string::npos);

        if (isBinary)
        {
            std::ifstream inFile(pathFile, std::ios::binary | std::ios::ate);
            if (inFile)
            {
                size_t fileSize = inFile.tellg();
                inFile.seekg(0, std::ios::beg);
                tabContents.resize(fileSize);
                inFile.read(&tabContents[0], fileSize);
                inFile.close();
            }
        }
        else
        {
            std::ifstream inFile(pathFile);
            if (inFile)
            {
                tabContents.assign((std::istreambuf_iterator<char>(inFile)),
                                    std::istreambuf_iterator<char>());
                inFile.close();
            }
        }

        std::cout << "File opened: " << pathFile << std::endl;
        NFD_FreePathU8(outPath);
    }
    else if (result == NFD_CANCEL)
    {
        std::cout << "User canceled open dialog" << std::endl;
    }
    else
    {
        std::cerr << "NFD error: " << NFD_GetError() << std::endl;
    }

    NFD_Quit();
}

/*
*  Additional function to save file(binary or text) - for not save as button(opening filedialog);
*/
void SaveFile(const std::string& path, const std::string& content)
{
    if (path.empty())
    {
        std::cerr << "SaveFile: empty path, file not saved" << std::endl;
        return;
    }

    bool isBinary = (path.find(".bin") != std::string::npos);

    std::ofstream outFile(path, isBinary ? (std::ios::binary | std::ios::trunc) : std::ios::trunc);

    if (!outFile)
    {
        std::cerr << "SaveFile: failed to open file: " << path << std::endl;
        return;
    }

    if (isBinary)
    {
        outFile.write(content.c_str(), content.size());
    }
    else
    {
        outFile << content;
    }

    outFile.close();
    std::cout << "File saved: " << path << std::endl;
}
