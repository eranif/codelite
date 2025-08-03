//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : csscopeconfdata.h
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

#ifndef __csscopeconfdata__
#define __csscopeconfdata__

#include "serialized_object.h"
#include <wx/string.h>

#define SCOPE_ENTIRE_WORKSPACE wxT("Entire Workspace")
#define SCOPE_ACTIVE_PROJECT   wxT("Active Project")

class CScopeConfData : public SerializedObject
{
	wxString m_cscopeFilepath;
	wxString m_scanScope;
	bool     m_rebuildDb;
	bool     m_buildRevertedIndex;

public:
	CScopeConfData();
    virtual ~CScopeConfData() = default;

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);

	void SetCscopeExe(const wxString& filepath) {
		this->m_cscopeFilepath = filepath;
	}
	const wxString& GetCscopeExe() const {
		return m_cscopeFilepath;
	}
	void SetScanScope(const wxString& scanScope) {
		this->m_scanScope = scanScope;
	}
	const wxString& GetScanScope() const {
		return m_scanScope;
	}
	void SetRebuildDbOption(const bool rebuild) {
		this->m_rebuildDb = rebuild;
	}
	bool GetRebuildOption() const {
		return m_rebuildDb;
	}
	void SetBuildRevertedIndexOption(const bool revertedIndex) {
		this->m_buildRevertedIndex = revertedIndex;
	}
	bool GetBuildRevertedIndexOption() const {
		return m_buildRevertedIndex;
	}
};
#endif // __csscopeconfdata__
