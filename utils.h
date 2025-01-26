#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <Windows.h>
#include <ctime>
#include <stdexcept>
#include <vector>

#define CONFIG_PATH L"Config.ini"
#define LOG_PATH L"D:\\beru-log.log"
#define OFF_PATH "D:\\close.wav"
#define PATH L"D:\\data.dat"
#define CODE_PAGE CP_ACP

namespace utils {
    // exception
    class RunTimeException : public std::runtime_error {
    public:
        RunTimeException(std::string str) : runtime_error(str) {}
    };

    // log func
    void OutputLog(std::string);
    void OutputLog(int, std::string);

    // string - wstring
    std::wstring StringToWstring(const std::string&);
    std::string WstringToString(const std::wstring&);


}

#endif