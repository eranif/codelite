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

    /// Gets all characters before the first occurrence of ch.
    /// Returns the whole string if ch is not found.
    static string BeforeFirst(const string& str, char ch);

    /// Gets all the characters after the first occurrence of ch.
    /// Returns the empty string if ch is not found.
    static string AfterFirst(const string& str, char ch);
};

#endif // STRINGUTILS_H
