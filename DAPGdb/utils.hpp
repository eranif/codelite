#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#define DELETE_PTR(p)      \
    if((p)) { delete(p); } \
    p = nullptr;

#endif // __UTILS_HPP__
