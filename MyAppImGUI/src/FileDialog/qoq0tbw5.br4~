#include "FileDialog.h"


void SaveFileDialog(HWND hwnd, const std::string& CurrentTabInfo, std::string& path)
{
    OPENFILENAMEA ofn;  // Structure for the file dialog
    char szFile[MAX_PATH] = "";  // Buffer to store the selected file name

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = "txt";

    if (GetSaveFileNameA(&ofn))
    {
        path = std::string(szFile);
        bool isBinary = (path.find(".bin") != NULL);

        std::ofstream outFile(szFile, isBinary ? (std::ios::binary | std::ios::trunc) : std::ios::trunc);
        if (outFile)
        {
            if (isBinary)
            {
                outFile.write(CurrentTabInfo.c_str(), CurrentTabInfo.size());
            }
            else
            {
                outFile << CurrentTabInfo; // Writing text normally
            }

            outFile.close();
            MessageBoxA(hwnd, szFile, "File Saved", MB_OK);
        }
        else
        {
            MessageBoxA(hwnd, "Error saving file", "File not saved", MB_OK);
        }
    }
}

void ShowOpenFileDialog(HWND hwnd, std::string &tabContents , std::string &pathFile)
{
    OPENFILENAMEA ofn;                // Structure for the file dialog
    char szFile[MAX_PATH] = (""); // Buffer to store the selected file name

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = ("Text Files (*.txt)\0*.txt\0Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = ("txt"); // Default extension is .txt

    

    if (GetOpenFileNameA(&ofn))
    {
        // For save file func
        pathFile = std::string(szFile);

        bool isBinary = (strstr(szFile, ".bin") != NULL);

        if (isBinary)
        {
            // Open and read binary file
            std::ifstream inFile(szFile, std::ios::binary | std::ios::in | std::ios::ate);
            if (inFile)
            {
                inFile.seekg(0, std::ios::end);
                size_t fileSize = inFile.tellg();
                inFile.seekg(0, std::ios::beg);

                std::string content(fileSize, '\0'); // Resize string to fit the content
                inFile.read(&content[0], fileSize);

#if _DEBUG
                std::cout << "Reading file binary"; // Just for debugging
                std::cout << "TabContents : " << tabContents << std::endl;
#endif
                
                inFile.close();
            }
        }
        else
        {
            // Open and read text file
            std::ifstream inFile(szFile);
            if (inFile)
            {
                std::string line;
                while (std::getline(inFile, line))
                {
                    tabContents += line + "\n";
                }
                inFile.close();
            }
        }
    }

    if (szFile[0] != '\0')
    {
        MessageBoxA(hwnd, szFile, ("File opened"), MB_OK);
    }
}

/*
*  Additional function to save file(binary or text) - for not save as button(opening filedialog);
*/
void SaveFile(HWND hwnd , const std::string& path, const std::string& content)
{
    if (!std::filesystem::exists(std::filesystem::path(path).parent_path())) { // Check directory instead
#if _DEBUG
        std::cerr << "Directory doesn't exist: " << std::filesystem::path(path).parent_path() << std::endl;
#endif
        MessageBoxA(hwnd, "No valid directory found", "File not saved", MB_OK);
        return;
    }

    bool isBinary = (path.find(".bin") != std::string::npos);

    std::ofstream outFile(path, isBinary ? (std::ios::binary | std::ios::trunc) : std::ios::trunc);

    if (outFile)
    {
        if (isBinary)
            outFile.write(content.c_str(), content.size());
        else
            outFile << content;

        outFile.close();
        MessageBoxA(hwnd, path.c_str(), "File Saved", MB_OK);
    }
    else
    {
        MessageBoxA(hwnd, "Error saving file", "File not saved", MB_OK);
    }
}
