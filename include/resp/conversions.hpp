#pragma once
#include <chrono>
#include <sstream>
#include <string>

namespace ignacionr::convert {

    std::chrono::system_clock::time_point USDateToTimePoint(const std::string &date)
    {
        std::tm tm = {};
        std::istringstream ss(date);
        char delimiter;

        // Parse string in MM/DD/YYYY format
        ss >> tm.tm_mon >> delimiter >> tm.tm_mday >> delimiter >> tm.tm_year;

        // Adjust for tm structure (months are [0, 11], years since 1900)
        tm.tm_mon -= 1;
        tm.tm_year -= 1900;

        // Convert to time_t
        std::time_t time = std::mktime(&tm);

        // Convert to std::chrono::system_clock::time_point
        return std::chrono::system_clock::from_time_t(time);
    }
}
