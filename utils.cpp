#include "utils.h"

std::wstring utils::StringToWstring(const std::string& sInput) {
    if (sInput.length() == 0)
        return L"";
    int iBufLen = MultiByteToWideChar(CODE_PAGE, 0, sInput.c_str(), -1, NULL, 0);
    wchar_t* buf = new wchar_t[iBufLen + 1];
    int iLen = MultiByteToWideChar(CODE_PAGE, 0, sInput.c_str(), -1, buf, iBufLen + 1);
    std::wstring sOutput(buf, iLen);
    delete[] buf;
    return sOutput;
}

std::string utils::WstringToString(const std::wstring& sInput) {
    if (sInput.length() == 0)
        return "";
    int iBufLen = WideCharToMultiByte(CODE_PAGE, 0, sInput.c_str(), -1, NULL, 0, NULL, NULL);
    char* buf = new char[iBufLen + 1];
    int iLen = WideCharToMultiByte(CODE_PAGE, 0, sInput.c_str(), -1, buf, iBufLen + 1, NULL, NULL);
    std::string sOutput(buf, iLen);
    delete[] buf;
    return sOutput;
}

//
//  FUNCTION: OutputLog(string)
//
//  PURPOSE: Output log to file "LOG_PATH"
//
void utils::OutputLog(std::string str) {
    // get time
    std::time_t tNowTime = std::time(nullptr);
    char cTime[26];
    std::tm* tm_time = std::localtime(&tNowTime);
    std::strftime(cTime, sizeof(cTime), "%Y-%m-%d %H:%M:%S", tm_time);
    str = std::string(cTime) + " " + str + "\r\n";

    //output
    HANDLE hFile = CreateFile(LOG_PATH, FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return;
    const char* data = str.c_str();
    DWORD bytesWritten;
    WriteFile(hFile, data, str.size(), &bytesWritten, NULL);
    CloseHandle(hFile);
}

//
//  FUNCTION: OutputLog(int, string)
//
//  PURPOSE: Output log prefixing a status keyword to the string. status=1 means [Info], status=2 means [Error], status=3 means [Info], but debug
//
void utils::OutputLog(int iStatus, std::string str) {
    switch (iStatus)
    {
    case 1:
    {
        utils::OutputLog("[Info] " + str);
        break;
    }
    case 2:
    {
        utils::OutputLog("[Error] " + str);
        break;
    }
    case 3:
    {
        #ifdef _DEBUG
        utils::OutputLog("[Info] " + str);
        #endif // _DEBUG
    }
    default:
        break;
    }
}