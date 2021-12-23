//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggerconfigtool.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef DEBUGGERCONFIGTOOL_H
#define DEBUGGERCONFIGTOOL_H

#include "codelite_exports.h"
#include "configtool.h"

class WXDLLIMPEXP_SDK DebuggerConfigTool : public ConfigTool
{
private:
    DebuggerConfigTool();

public:
    ~DebuggerConfigTool();
    static DebuggerConfigTool* Get();
};
#endif // DEBUGGERCONFIGTOOL_H
