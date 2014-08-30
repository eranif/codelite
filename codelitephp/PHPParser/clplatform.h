#ifndef CLPLATFORM_H
#define CLPLATFORM_H

#include <wx/string.h>
#include <wx/wxchar.h>

class clPlatform
{
public:
    enum {
        Gtk,
        Windows,
        Mac,
        Unknown
    };
    static wxChar PathSeparator;
public:
    static bool OS();
};

#endif // CLPLATFORM_H
