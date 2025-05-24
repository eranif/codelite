//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debugcoredumpinfo.h
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

#ifndef __debugcoredumpinfo__
#define __debugcoredumpinfo__

#include <wx/arrstr.h>
#include <wx/string.h>
#include "serialized_object.h"
#include "codelite_exports.h"

class DebugCoreDumpInfo : public SerializedObject
{
public:
	DebugCoreDumpInfo() : m_selectedDbg(0) {}
	~DebugCoreDumpInfo() {}

	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);

	void SetCoreFilepaths(const wxArrayString& coreFilepaths) {
		this->m_coreFilepaths = coreFilepaths;
	}
	void SetExeFilepaths(const wxArrayString& exeFilepaths) {
		this->m_exeFilepaths = exeFilepaths;
	}
	void SetSelectedDbg(int selectedDbg) { this->m_selectedDbg = selectedDbg; }
	void SetWDs(const wxArrayString& wds) {
		this->m_wds = wds;
	}

	const wxArrayString& GetCoreFilepaths() const {
		return m_coreFilepaths;
	}
	const wxArrayString& GetExeFilepaths() const {
		return m_exeFilepaths;
	}
	int GetSelectedDbg() const { return m_selectedDbg; }
	const wxArrayString& GetWds() const {
		return m_wds;
	}

protected:
	wxArrayString m_coreFilepaths;
	wxArrayString m_exeFilepaths;
	wxArrayString m_wds;
	int m_selectedDbg;
};
#endif // __debugcoredumpinfo__
