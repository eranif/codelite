#pragma once

#include "codelite_exports.h"
#include "wx/string.h"

#include <vector>

class WXDLLIMPEXP_CL StackWalker
{
public:
    static void Dump(const std::vector<wxString>& prefix, bool dumpCurrentThread = true);
    static void Initialise();
    static void Shutdown();
};
