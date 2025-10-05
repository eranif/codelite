//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : importfilessettings.cpp
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
#include "importfilessettings.h"

ImportFilesSettings::ImportFilesSettings()
    : m_fileMask("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc;*.hxx;*.h++")
    , m_flags(0)
{
}

void ImportFilesSettings::DeSerialize(Archive& arch)
{
    arch.Read(wxT("m_fileMask"), m_fileMask);
    arch.Read(wxT("m_flags"), m_flags);
    arch.Read(wxT("m_baseDir"), m_baseDir);
}

void ImportFilesSettings::Serialize(Archive& arch)
{
    arch.Write(wxT("m_fileMask"), m_fileMask);
    arch.Write(wxT("m_flags"), m_flags);
    arch.Write(wxT("m_baseDir"), m_baseDir);
}
