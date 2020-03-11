#include <filesystem>
#include <iostream>
#include <string>
#include <Windows.h>

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

int main(int argc, char *argv[])
{
    std::string tp;
    /* dir option */
    if (argv[1] != NULL)
    {
        tp = argv[1];
    }
    else
    {
        tp = ".";
    }

    for (const std::filesystem::directory_entry &i : std::filesystem::directory_iterator(tp))
    {
        if (std::filesystem::is_regular_file(i.path()))
        {
            std::cout << UTF8toSjis(i.path().filename().string()) << std::endl;
        }
        else if (std::filesystem::is_directory(i.path()))
        {
            /* blue sky -> 36m */
            std::cout << "\033[36m"
                      << UTF8toSjis(i.path().filename().string())
                      << "\033[m"
                      << std::endl;
        }
    }
}
