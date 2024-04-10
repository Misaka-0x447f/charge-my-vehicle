#pragma once

#include <sstream>
#include <string>

namespace lang
{
    template <typename T>
    const char* toString(const T &value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str().c_str();
    }
}
