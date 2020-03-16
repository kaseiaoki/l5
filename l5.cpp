#include <filesystem>
#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
namespace fs = std::filesystem;

template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}

std::string SjistoUTF8(std::string srcSjis)
{
    int lenghtUnicode = MultiByteToWideChar(CP_THREAD_ACP, 0, srcSjis.c_str(), srcSjis.size() + 1, NULL, 0);

    wchar_t *bufUnicode = new wchar_t[lenghtUnicode];

    MultiByteToWideChar(CP_THREAD_ACP, 0, srcSjis.c_str(), srcSjis.size() + 1, bufUnicode, lenghtUnicode);

    int lengthUTF8 = WideCharToMultiByte(CP_UTF8, 0, bufUnicode, -1, NULL, 0, NULL, NULL);

    char *bufUTF8 = new char[lengthUTF8];

    WideCharToMultiByte(CP_UTF8, 0, bufUnicode, lenghtUnicode - 1, bufUTF8, lengthUTF8, NULL, NULL);

    std::string strUTF8(bufUTF8);

    delete bufUnicode;
    delete bufUTF8;

    return strUTF8;
}

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

std::string getFileType(fs::file_type type)
{
    switch (type)
    {
    case fs::file_type::none:
        return "none";
        break;
    case fs::file_type::not_found:
        return "not found";
        break;
    case fs::file_type::regular:
        return "regular file";
        break;
    case fs::file_type::directory:
        return "directory file";
        break;
    case fs::file_type::symlink:
        return "symbolic link file";
        break;
    case fs::file_type::block:
        return "block special file";
        break;
    case fs::file_type::character:
        return "character special file";
        break;
    case fs::file_type::fifo:
        return "FIFO or pipe file";
        break;
    case fs::file_type::socket:
        return "socket file";
        break;
    case fs::file_type::unknown:
        return "unknown type file";
        break;
    default:
        return "implementation-defined file type";
        break;
    }
}

bool getFilenames(std::string path, std::vector<std::string> &dirNames, std::vector<std::string> &fileNames)
{
    for (const fs::directory_entry &i : fs::directory_iterator(path))
    {
        std::string fileName = i.path().filename().string();
        if (fs::is_regular_file(i.path()))
        {
            fileNames.push_back(fileName);
        }
        else if (fs::is_directory(i.path()))
        {
            dirNames.push_back(fileName);
        }
    }
    return true;
}

void printFile(std::vector<std::string> &Names, std::string path)
{
    if (path.back() != '/')
    {
        path += "/";
    }

    for (int i = 0; i < Names.size(); ++i)
    {
        std::string name = path + Names.at(i);
        fs::file_status status = fs::status(path + UTF8toSjis(name));
        fs::file_type fileType = status.type();
        std::string fileTypeString = getFileType(fileType);
        int size = fs::file_size(path + Names.at(i));
        auto ftime = fs::last_write_time(UTF8toSjis(name));
        std::time_t tt = to_time_t(ftime);
        std::tm *gmt = std::gmtime(&tt);
        std::stringstream buffer;
        buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M");
        std::string formattedFileTime = buffer.str();
        std::cout << fileTypeString << "  " << formattedFileTime << "  " << size << "  " << UTF8toSjis(Names.at(i)) << std::endl;
    }
}

void printDir(std::vector<std::string> &Names, std::string path)
{

    if (path.back() != '/')
    {
        path += "/";
    }

    for (int i = 0; i < Names.size(); ++i)
    {
        auto ftime = fs::last_write_time(UTF8toSjis(path + Names.at(i)));
        std::time_t tt = to_time_t(ftime);
        std::tm *gmt = std::gmtime(&tt);
        std::stringstream buffer;
        buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M");
        std::string formattedFileTime = buffer.str();
        std::cout << "directory  " << formattedFileTime << "  " << UTF8toSjis(Names.at(i)) << std::endl;
    }
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

    /* get file names */
    std::vector<std::string> fileNames;
    std::vector<std::string> dirNames;
    getFilenames(targetPath, dirNames, fileNames);

    /* print */
    printDir(dirNames, targetPath);
    printFile(fileNames, targetPath);
}
