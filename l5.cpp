#include <filesystem>
#include <iostream>

int main()
{
    std::filesystem::path path = std::filesystem::current_path();
    for (const std::filesystem::directory_entry &i : std::filesystem::directory_iterator("."))
    {
        if (std::filesystem::is_regular_file(i.path()))
        {
            std::cout << "file: " << i.path().filename().string() << std::endl;
        }
        else if (std::filesystem::is_directory(i.path()))
        {
            std::cout << "dir : " << i.path().filename().string() << std::endl;
        }
    }
}
