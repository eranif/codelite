//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debugcoredumpinfo.cpp
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

#include "debugcoredumpinfo.h"

void DebugCoreDumpInfo::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_coreName"), m_coreFilepaths);
    arch.Read(wxT("m_exeFilePaths"), m_exeFilepaths);
    arch.Read(wxT("m_wds"), m_wds);
    arch.Read(wxT("m_selectedDbg"), m_selectedDbg);
}

void DebugCoreDumpInfo::Serialize(Archive& arch)
{
    arch.Write(wxT("m_coreName"), m_coreFilepaths);
    arch.Write(wxT("m_exeFilePaths"), m_exeFilepaths);
    arch.Write(wxT("m_wds"), m_wds);
    arch.Write(wxT("m_selectedDbg"), m_selectedDbg);
}
