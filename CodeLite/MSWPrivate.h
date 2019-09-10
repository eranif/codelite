#ifndef MSWPRIVATE_H
#define MSWPRIVATE_H

#define YY_NEVER_INTERACTIVE 1
#define CURRENT_GCC_VERSION ((__GNUC__*1000)+(__GNUC_MINOR__*100))
#define GCC_VERSION(major, minor) ((major*1000)+(minor*100))

#if CURRENT_GCC_VERSION < GCC_VERSION(4,9)
#include <sstream>
namespace std {
    template<typename T>
    std::string to_string(T x) {
        std::stringstream ss;
        ss << x;
        return ss.str();
    }
}
#endif

#endif // MSWPRIVATE_H
