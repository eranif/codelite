#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>

using namespace std;
#define UNUSED(x) ((void)x)

class StringUtils
{
public:
    static void Rtrim(wstring& str);
    static void Rtrim(string& str);
};

#endif // STRINGUTILS_H
