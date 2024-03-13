#include "Util.h"

#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>

#include <iostream>
#include <filesystem>

namespace AirlineReservationSystem::Util
{
    // Convert SQL datetime string to integer timestamp
    int DatetimeToInt(const std::string& datetimeString)
    {
        std::tm timeStruct = {}; // Initialize with zeros

        // Parse the datetime string
        std::istringstream ss(datetimeString);
        ss >> std::get_time(&timeStruct, "%Y-%m-%d %H:%M:%S");

        if (ss.fail())
        {
            std::cerr << "Error parsing datetime string: " << datetimeString << std::endl;
            return -1; // Return an error value
        }

        // Calculate total seconds since epoch (January 1, 1970)
        std::time_t timestamp = std::mktime(&timeStruct);

        return static_cast<int>(timestamp);
    }

    int DatetimeToInt(int year, int month, int day, int minutes)
    {
        // Combine components into an integer
        return year * 100000000 + month * 1000000 + day * 10000 + minutes;
    }

    std::string IntToDatetime(int timestamp)
    {
        std::time_t t = static_cast<std::time_t>(timestamp);
        std::tm* ltm = std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(ltm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    void OpenFileWithDefaultApp(const std::string& filename)
    {
        namespace fs = std::filesystem;
        const fs::path filePath(filename);

        if (fs::exists(filePath))
        {
            // File exists, handle accordingly
            fs::path absolutePath = fs::absolute(filePath);
            std::cout << "Opening file: " << absolutePath << std::endl;

            // Use platform-specific command to open the file
            std::string command;
#ifdef _WIN32 // Windows
            command = "explorer \"" + absolutePath.string() + "\"";
#elif __APPLE__ // macOS
            command = "open \"" + absolutePath.string() + "\"";
#else // Linux and other Unix-like systems
            command = "xdg-open \"" + absolutePath.string() + "\"";
#endif

            /*int result = std::system(command.c_str());
            if (result != 0)
            {
                std::cerr << "Error opening the file." << std::endl;
            }*/
        }
        else
        {
            std::cerr << "File does not exist: " << filePath << std::endl;
        }
    }

}