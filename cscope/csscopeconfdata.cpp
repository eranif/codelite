//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : csscopeconfdata.cpp
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

#include "csscopeconfdata.h"
#include <wx/stdpaths.h>
CScopeConfData::CScopeConfData()
		: m_cscopeFilepath(wxT(""))
		, m_scanScope(SCOPE_ENTIRE_WORKSPACE)
		, m_rebuildDb(true)
		, m_buildRevertedIndex(false)
{
#ifdef __WXMSW__
	// try the executable path
	wxFileName codelitePath( wxStandardPaths::Get().GetExecutablePath() );
	wxString   exePath;
	exePath << codelitePath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR) << wxT("cscope.exe");

	if( wxFileName::FileExists(exePath) ) {
		m_cscopeFilepath = exePath;
	} else {
		m_cscopeFilepath = wxT("cscope.exe");
	}
#else
	m_cscopeFilepath = wxT("cscope");
#endif
}

CScopeConfData::~CScopeConfData()
{
}

void CScopeConfData::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_cscopeFilepath"), m_cscopeFilepath);
	arch.Read(wxT("m_scanScope"), m_scanScope);
	arch.Read(wxT("m_rebuildDb"), m_rebuildDb);
	arch.Read(wxT("m_buildRevertedIndex"), m_buildRevertedIndex);
}

void CScopeConfData::Serialize(Archive& arch)
{
	arch.Write(wxT("m_cscopeFilepath"), m_cscopeFilepath);
	arch.Write(wxT("m_scanScope"), m_scanScope);
	arch.Write(wxT("m_rebuildDb"), m_rebuildDb);
	arch.Write(wxT("m_buildRevertedIndex"), m_buildRevertedIndex);
}
