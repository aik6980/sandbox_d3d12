#include "string_pool.h"

#include "common_cpp.h"

const char* Pooled_string(const string& s)
{
    static unordered_set<string> interned;
    return interned.insert(s).first->c_str();
}
