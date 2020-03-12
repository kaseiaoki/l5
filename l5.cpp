#include <filesystem>
#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>

std::string UTF8toSjis(std::string srcUTF8)
{
    /* convert unicode */
    int lenghtUnicode = MultiByteToWideChar(CP_UTF8, 0, srcUTF8.c_str(), srcUTF8.size() + 1, NULL, 0);

    wchar_t *bufUnicode = new wchar_t[lenghtUnicode];

    MultiByteToWideChar(CP_UTF8, 0, srcUTF8.c_str(), srcUTF8.size() + 1, bufUnicode, lenghtUnicode);

    /* convert SJis */
    int lengthSJis = WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, -1, NULL, 0, NULL, NULL);

    char *bufShiftJis = new char[lengthSJis];

    WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, lenghtUnicode + 1, bufShiftJis, lengthSJis, NULL, NULL);

    std::string strSJis(bufShiftJis);

    /* buf delete */
    delete bufUnicode;
    delete bufShiftJis;

    return strSJis;
}

bool getFilenames(std::string path, std::vector<std::string> &dirNames, std::vector<std::string> &fileNames)
{
    for (const std::filesystem::directory_entry &i : std::filesystem::directory_iterator(path))
    {
        if (std::filesystem::is_regular_file(i.path()))
        {
            UTF8toSjis(i.path().filename().string());
            fileNames.push_back(UTF8toSjis(i.path().filename().string()));
        }
        else if (std::filesystem::is_directory(i.path()))
        {
            dirNames.push_back(UTF8toSjis(i.path().filename().string()));
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    std::string targetPath;
    /* dir option */
    if (argv[1] != NULL)
    {
        targetPath = argv[1];
    }
    else
    {
        targetPath = ".";
    }

    std::vector<std::string> fileNames;
    std::vector<std::string> dirNames;

    /* get filenames */
    int size = getFilenames(targetPath, dirNames, fileNames);
    /* display all files */
    for (int i = 0; i < dirNames.size(); ++i)
    {
        std::cout << "/" << dirNames.at(i) << std::endl;
    }
    for (int i = 0; i < fileNames.size(); ++i)
    {
        std::cout << fileNames.at(i) << std::endl;
    }
}
