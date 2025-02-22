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
        bool isBinary = (strstr(szFile, ".bin") != NULL);

        if (isBinary)
        {
            std::ofstream outFile(szFile, std::ios::out | std::ios::binary | std::ios::trunc);
            if (outFile)
            {
                size_t length = CurrentTabInfo.length();
                //outFile.write(reinterpret_cast<const char*>(&length), sizeof(size_t));  // Writing length as binary
                outFile.write(CurrentTabInfo.c_str(), length);  // Writing the content as binary
                outFile.close();
                MessageBoxA(hwnd, szFile, "File Saved", MB_OK);  // Show path in MessageBox
            }
            else
            {
                MessageBoxA(hwnd, szFile, "File not saved", MB_OK);
            }
        }
        else
        {
            // Save as a text file
            std::ofstream outFile(szFile, std::ios::out | std::ios::trunc);
            if (outFile)
            {
                for (const auto& c : CurrentTabInfo)
                {
                    outFile << c;
                }
                outFile.close();
                MessageBoxA(hwnd, szFile, "File Saved", MB_OK);  // Show path in MessageBox
            }
            else
            {
                MessageBoxA(hwnd, szFile, "File not saved", MB_OK);
            }
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
                size_t fileSize = inFile.tellg();
                inFile.seekg(0, std::ios::beg);

                tabContents.resize(fileSize); // Resize string buffer
                if (fileSize > 0)
                {
#if _DEBUG
                    std::cout << "Reading file binary"; // Just for debugging
#endif
                    inFile.read(&tabContents[0], fileSize);
#if _DEBUG
                    std::cout << "TabContents : " << tabContents << std::endl;
#endif
                }
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
    if (!std::filesystem::exists(path)) { // Check if directory exists
#if _DEBUG
        std::cerr << "Directory doesn't exist: " << path << std::endl;
#endif
        MessageBoxA(hwnd, "No path to file found", "File not saved", MB_OK);
        return;
    }

    bool isBinary = (path.find(".bin") != std::string::npos);

    std::ofstream outFile(path, isBinary ? (std::ios::binary | std::ios::trunc) : std::ios::trunc);
    if (outFile)
    {
        if (isBinary) {
            for (const auto& c : content)
            {
                outFile << c;
            }
        }
        else {
            outFile.write(content.c_str(), content.size());
        }

        outFile.close();
        MessageBoxA(hwnd, path.c_str(), "File Saved", MB_OK);
    }
}
