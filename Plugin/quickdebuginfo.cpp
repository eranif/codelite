//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : quickdebuginfo.cpp
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

#include "quickdebuginfo.h"
QuickDebugInfo::QuickDebugInfo()
    : m_selectedDbg(0)
{
}

QuickDebugInfo::~QuickDebugInfo() {}

void QuickDebugInfo::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_arguments"), m_arguments);
    arch.Read(wxT("m_exeFilepaths"), m_exeFilepaths);
    arch.Read(wxT("m_selectedDbg"), m_selectedDbg);
    arch.Read(wxT("m_startCmds"), m_startCmds);
    arch.Read(wxT("m_wds"), m_wds);
    arch.Read(wxT("m_alternateDebuggerExec"), m_alternateDebuggerExec);
}

void QuickDebugInfo::Serialize(Archive& arch)
{
    arch.Write(wxT("m_arguments"), m_arguments);
    arch.Write(wxT("m_exeFilepaths"), m_exeFilepaths);
    arch.Write(wxT("m_selectedDbg"), m_selectedDbg);
    arch.Write(wxT("m_startCmds"), m_startCmds);
    arch.Write(wxT("m_wds"), m_wds);
    arch.Write(wxT("m_alternateDebuggerExec"), m_alternateDebuggerExec);
}
