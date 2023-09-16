#ifndef CODELITECC_STRING_UTILS_HPP
#define CODELITECC_STRING_UTILS_HPP
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>

typedef std::vector<std::string> StringVec_t;

char* normalize_path(const char* src, size_t src_len);
bool ends_with(const std::string& s, const std::string& e);
bool is_source_file(const std::string& filename, std::string& fixed_file_name);
void* my_memrchr(const void* buf, int c, size_t num);
std::string& trim(std::string& s);

inline bool ends_with(const std::string& s, const std::string& e)
{
    size_t where = s.rfind(e);
    return (where == std::string::npos ? false : ((s.length() - where) == e.length()));
}

inline bool is_source_file(const std::string& filename, std::string& fixed_file_name)
{
    StringVec_t extensions;
    extensions.push_back(".cpp");
    extensions.push_back(".cxx");
    extensions.push_back(".cc");
    extensions.push_back(".c");

    for(size_t n = 0; n < extensions.size(); ++n) {
        if(ends_with(filename, extensions.at(n))) {
            fixed_file_name = filename;

#ifdef _WIN32
            std::replace(fixed_file_name.begin(), fixed_file_name.end(), '/', '\\');
#endif
            char* ret = normalize_path(fixed_file_name.c_str(), fixed_file_name.length());
            fixed_file_name = ret;
            free(ret);

            // rtrim
            fixed_file_name.erase(0, fixed_file_name.find_first_not_of("\t\r\v\n\" "));

            // ltrim
            fixed_file_name.erase(fixed_file_name.find_last_not_of("\t\r\v\n\" ") + 1);
            return true;
        }
    }
    fixed_file_name.clear();
    return false;
}

inline char* normalize_path(const char* src, size_t src_len)
{
    std::string strpath = src;
    size_t where = strpath.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    bool has_drive = (where == 0 && strpath.length() > 1 && strpath.at(1) == ':');

    char* res;
    size_t res_len;

    const char* ptr = src;
    const char* end = &src[src_len];
    const char* next;

    if(!has_drive && (src_len == 0 || src[0] != '/')) {

        // relative path

        char pwd[4096];
        size_t pwd_len;

        if(getcwd(pwd, sizeof(pwd)) == NULL) {
            return NULL;
        }

        pwd_len = strlen(pwd);
        std::replace(pwd, pwd + pwd_len, '\\', '/');

        res = (char*)malloc(pwd_len + 1 + src_len + 1);
        memcpy(res, pwd, pwd_len);
        res_len = pwd_len;
    } else {
        res = (char*)malloc((src_len > 0 ? src_len : 1) + 1);
        res_len = 0;
    }

    for(ptr = src; ptr < end; ptr = next + 1) {
        size_t len;
        next = (char*)memchr(ptr, '/', end - ptr);
        if(next == NULL) {
            next = end;
        }
        len = next - ptr;
        switch(len) {
        case 2:
            if(ptr[0] == '.' && ptr[1] == '.') {
                const char* slash = (char*)my_memrchr(res, '/', res_len);
                if(slash != NULL) {
                    res_len = slash - res;
                }
                continue;
            }
            break;
        case 1:
            if(ptr[0] == '.') {
                continue;
            }
            break;
        case 0:
            continue;
        }

        if(res_len == 0 && !has_drive)
            res[res_len++] = '/';
        else if(res_len)
            res[res_len++] = '/';

        memcpy(&res[res_len], ptr, len);
        res_len += len;
    }

    if(res_len == 0) {
        res[res_len++] = '/';
    }
    res[res_len] = '\0';
    return res;
}

inline void* my_memrchr(const void* buf, int c, size_t num)
{
    char* pMem = (char*)buf;

    for(;;) {
        if(num-- == 0) {
            return NULL;
        }

        if(pMem[num] == (unsigned char)c) {
            break;
        }
    }

    return (void*)(pMem + num);
}

inline bool contains(const std::string& s, const std::string& what) { return s.find(what) != std::string::npos; }

inline std::string& trim(std::string& s)
{
    // Trim whitespace from right and left
    static std::string trimLeftString("\r\n\t\v ");
    static std::string trimRightString("\r\n\t\v ");
    s.erase(0, s.find_first_not_of(trimLeftString));
    s.erase(s.find_last_not_of(trimRightString) + 1);
    return s;
}

#endif // CODELITECC_STRING_UTILS_HPP
