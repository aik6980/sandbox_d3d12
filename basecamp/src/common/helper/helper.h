#pragma once

// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x

// template <typename T, typename... Rest>
// inline void Hash_combine(std::size_t& seed, T const& v, Rest&&... rest)
//{
//     std::hash<T> hasher;
//     seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//     (int[]){0, (Hash_combine(seed, std::forward<Rest>(rest)), 0)...};
// }

// templated version of Case_insensitive_equal so it could work with both char and wchar_t
template <typename charT>
struct Case_insensitive_equal {
    Case_insensitive_equal(const std::locale& loc) : loc_(loc) {}
    bool operator()(charT ch1, charT ch2) { return std::tolower(ch1, loc_) == std::tolower(ch2, loc_); }

  private:
    const std::locale& loc_;
};

// find substring (case insensitive)
template <typename T>
int Case_insensitive_find_substr(const T& str1, const T& str2, const std::locale& loc = std::locale())
{
    typename T::const_iterator it = std::search(str1.begin(), str1.end(), str2.begin(), str2.end(), Case_insensitive_equal<typename T::value_type>(loc));
    if (it != str1.end()) {
        return it - str1.begin();
    }
    else {
        return -1; // not found
    }
}

template <typename T>
inline constexpr T Align_up(T val, T align)
{
    return (val + align - 1) / align * align;
}
