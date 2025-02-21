#include "FileDialog.h"

void SaveFileDialog(HWND hwnd, const std::string &CurrentTabInfo)
{
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = _T(""); // Buffer to store the selected file name

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = _T("Text Files (*.txt)\0*.txt\0Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = _T("txt");

    if (GetSaveFileName(&ofn))
    {
        bool isBinary = (_tcsstr(szFile, _T(".bin")) != NULL);

        if (isBinary)
        {
            std::ofstream outFile(szFile, std::ios::out | std::ios::binary | std::ios::trunc);
            if (outFile)
            {
                size_t length = CurrentTabInfo.length();
                // outFile.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
                outFile.write(CurrentTabInfo.c_str(), length);
                outFile.close();
            }
        }
        else
        {
            // Save as a text file
            std::ofstream outFile(szFile);
            if (outFile)
            {
                for (const auto &content : CurrentTabInfo)
                {
                    outFile << content << std::endl;
                }
                outFile.close();
            }
        }
    }
    // Fix to show message
    if (szFile[0] != '\0')
    {
        MessageBox(hwnd, szFile, _T("File Saved"), MB_OK);
    }
}

void ShowOpenFileDialog(HWND hwnd, std::string &tabContents)
{
    OPENFILENAME ofn;                // Structure for the file dialog
    TCHAR szFile[MAX_PATH] = _T(""); // Buffer to store the selected file name

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = _T("Text Files (*.txt)\0*.txt\0Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0");
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = _T("txt"); // Default extension is .txt

    if (GetOpenFileName(&ofn))
    {
        bool isBinary = (_tcsstr(szFile, _T(".bin")) != NULL);

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
        MessageBox(hwnd, szFile, _T("File opened"), MB_OK);
    }
}

/*
*  Additional function to save file(binary or text) - for not save as button(opening filedialog);
*/
void SaveFileToPath(const std::string& path, const std::string& content) {
    // Check if directory exist
    std::filesystem::path filePath(path);
    if (!std::filesystem::exists(filePath.parent_path())) {
        std::cerr << "Directory don`t exist!" << std::endl;
        return;
    }

    // Check if file has .bin or .txt 
    bool isBinary = path.find(".bin") != std::string::npos;

    if (isBinary) {
        std::ofstream outFile(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (outFile) {
            size_t length = content.length();
            //outFile.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
            outFile.write(content.c_str(), length);
            outFile.close();
            MessageBoxA(hwnd, path.c_str(), "File Saved", MB_OK);  // Show path in MessageBox
        }
        else {
            MessageBoxA(hwnd, path.c_str(), "File not saved" , MB_OK);
        }
    }
    else {
        // Збереження як текстовий файл
        std::ofstream outFile(path, std::ios::out | std::ios::trunc);
        if (outFile) {
            outFile << content;
            outFile.close();
            MessageBoxA(hwnd, path.c_str(), "File Saved", MB_OK);   // Show path in MessageBox
        }
        else {
            MessageBoxA(hwnd, path.c_str(), "File not saved", MB_OK);
        }
    }
}
