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

template <typename T, std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
unsigned int digit(const T &num)
{
    if (0 == num)
        return 1;
    unsigned int digit = 0;
    for (T i = num; i != 0; i /= 10, digit++)
        ;
    return digit;
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
    /* need left padding conforming with "symbolic link file"*/
    switch (type)
    {
    case fs::file_type::none:
        return "              none";
        break;
    case fs::file_type::not_found:
        return "         not found";
        break;
    case fs::file_type::regular:
        return "      regular file";
        break;
    case fs::file_type::directory:
        return "    directory file";
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

std::string toFileTime(std::string path)
{
    auto ftime = fs::last_write_time(fs::u8path(path));
    std::time_t tt = to_time_t(ftime);
    std::tm *gmt = std::gmtime(&tt);
    std::stringstream buffer;
    buffer << std::put_time(gmt, "%d/%m/%Y %H:%M");
    std::string formattedFileTime = buffer.str();
    return formattedFileTime;
}

std::string toFormattedFileSize(std::string path)
{
    /* file size */
    int size = -1;
    /* closely max Capacity of win systeme -> 238822617088 */
    /* need left padding for 12 times */
    std::string sizeSpace = "            ";
    if (fs::is_regular_file(path))
    {
        sizeSpace = " ";
        size = fs::file_size(path);
        int sizeDigit = digit(size);
        /* closely max Capacity of win systeme -> 238822617088*/
        int maxDigit = 13;
        for (int i = 0; i < (13 - sizeDigit); i++)
        {
            sizeSpace += " ";
        }
    }
    return sizeSpace + std::to_string(size);
}

bool getFilenames(std::string path, std::vector<std::string> &fileNames)
{
    for (const fs::directory_entry &i : fs::directory_iterator(path))
    {
        std::string fileName = i.path().filename().string();
        fileNames.push_back(fileName);
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
        /* file type */
        fs::file_status status = fs::status(name);
        fs::file_type fileType = status.type();
        std::string fileTypeString = getFileType(fileType);
        /* size */
        std::string size = toFormattedFileSize(name);
        /* last write type */
        std::string formattedFileTime = toFileTime(name);
        /* print */
        std::cout << fileTypeString << "  " << formattedFileTime << size << "  " << UTF8toSjis(Names.at(i)) << std::endl;
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
    getFilenames(targetPath, fileNames);

    /* print */
    printFile(fileNames, targetPath);
}
