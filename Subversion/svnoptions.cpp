//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : svnoptions.cpp
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
 #include "svnoptions.h"

SvnOptions::SvnOptions()
: m_flags(SvnUseIcons | SvnKeepIconsUpdated | SvnAutoAddFiles)
, m_exePath(wxT("svn"))
, m_pattern(wxT("*.o;*.obj;*.exe;*.lib;*.so;*.dll;*.a;*.dynlib;*.exp;*.ilk;*.pdb;*.d;*.tags;*.suo;*.ncb;*.bak;*.orig"))
, m_diffCmd(wxEmptyString)
, m_diffArgs(wxEmptyString)
, m_keepTagUpToDate(true)
{
}

SvnOptions::~SvnOptions()
{
}

void SvnOptions::Serialize(Archive &arch)
{
	arch.Write(wxT("m_flags"),           m_flags);
	arch.Write(wxT("m_exePath"),         m_exePath);
	arch.Write(wxT("m_pattern"),         m_pattern);
	arch.Write(wxT("m_diffCmd"),         m_diffCmd);
	arch.Write(wxT("m_diffArgs"),        m_diffArgs);
	arch.Write(wxT("m_keepTagUpToDate"), m_keepTagUpToDate);
}

void SvnOptions::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_flags"),           m_flags);
	arch.Read(wxT("m_exePath"),         m_exePath);
	arch.Read(wxT("m_pattern"),         m_pattern);
	arch.Read(wxT("m_diffCmd"),         m_diffCmd);
	arch.Read(wxT("m_diffArgs"),        m_diffArgs);
	arch.Read(wxT("m_keepTagUpToDate"), m_keepTagUpToDate);
}

