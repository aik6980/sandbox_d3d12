#include "helper.h"

// Convert a wide Unicode string to an UTF8 string
std::string To_string(const std::wstring& wstr)
{
    if (wstr.empty()) {
        return std::string();
    }

    int         size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring To_wstring(const std::string& str)
{
    if (str.empty()) {
        return std::wstring();
    }

    int          size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
    return wstr;
}
