#pragma once

// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x

template <typename T, typename... Rest>
inline void has_combine(std::size_t& seed, T const& v, Rest&&... rest)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (int[]){0, (hash_combine(seed, std::forward<Rest>(rest)), 0)...};
}
