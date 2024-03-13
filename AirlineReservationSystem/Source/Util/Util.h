#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <string>

namespace AirlineReservationSystem::Util
{
    // Convert SQL datetime string to integer timestamp
    int DatetimeToInt(const std::string& datetimeString);

    int DatetimeToInt(int year, int month, int day, int minutes);

    std::string IntToDatetime(int timestamp);

    void OpenFileWithDefaultApp(const std::string& filename);

}