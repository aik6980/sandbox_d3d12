#pragma once

namespace DBG {
template <typename T>
auto Argument(const T val)
{
    return val;
}

template <typename T>
const T* Argument(const std::basic_string<T>& val)
{
    return val.c_str();
}

template <typename... Args>
int32_t StringPrint(char* const buffer, size_t buffer_count, char const* const fmt, Args const&... args)
{
    const int32_t result = snprintf(buffer, buffer_count, fmt, Argument(args)...);

    assert(result != -1);
    return result;
}

template <typename... Args>
int32_t StringPrint(wchar_t* const buffer, size_t buffer_count, wchar_t const* const fmt, Args const&... args)
{
    const int32_t result = swprintf(buffer, buffer_count, fmt, Argument(args)...);

    assert(result != -1);
    return result;
}

template <typename T, typename... Args>
std::basic_string<T> Format(T const* const fmt, Args const&... args)
{
    std::basic_string<T> buffer;
    const int32_t        size = StringPrint(nullptr, 0, fmt, args...);
    if (size > buffer.size()) {
        buffer.resize(size);
        StringPrint(&buffer[0], buffer.size() + 1, fmt, args...);
    }

    return buffer;
}

template <typename... Args>
void OutputString(char const* const fmt, Args const&... args)
{
    std::string buf = Format(fmt, args...);
    OutputDebugStringA(buf.c_str());
}

template <typename... Args>
void OutputString(wchar_t const* const fmt, Args const&... args)
{
    std::wstring buf = Format(fmt, args...);
    OutputDebugStringW(buf.c_str());
}

}; // namespace DBG
