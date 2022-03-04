#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#ifdef __linux__
#include <unistd.h>
#elif __Wi32
#include <direct.h>
#endif
#include <exception>
#include <string>
#include <string_view>
#include <cstdio>


namespace util {

class DirNotFound : public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "Directory not found";
    }
};

class Utility
{
public:
    static std::string currentDir()
    {
        char buffer[FILENAME_MAX];

        auto currDir = getcwd(buffer, FILENAME_MAX);

        return std::string(currDir);
    }

    static void setCurrendDir(std::string_view dirName)
    {
        std::cout << "Changing dir to: " << dirName << std::endl;
        int res = chdir(dirName.data());

        if (res != 0)
            throw DirNotFound();
    }

    static std::string excuteCommand(std::string_view command)
    {
        const int BUFFER_SIZE = 256;
        char buffer[BUFFER_SIZE];
        std::string output = "";
        FILE *pipe = popen(command.data(), "r");

        if (!pipe)
            return "Unknown error";
        while (!feof(pipe))
        {
            if (fgets(buffer, BUFFER_SIZE, pipe))
                output += buffer;
        }

        pclose(pipe);

        return output;
    }
};

}

#endif // UTILITY_H
