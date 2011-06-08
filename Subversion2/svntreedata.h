//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : svntreedata.h
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

#ifndef SVNTREEDATA_H
#define SVNTREEDATA_H

#include <wx/treectrl.h>

class SvnTreeData : public wxTreeItemData
{
public:
	enum SvnNodeType {
		SvnNodeTypeInvalid = -1,
		SvnNodeTypeRoot,
		SvnNodeTypeModifiedRoot,
		SvnNodeTypeUnversionedRoot,
		SvnNodeTypeDeletedRoot,
		SvnNodeTypeAddedRoot,
		SvnNodeTypeConflictRoot,
		SvnNodeTypeFile,
		SvnNodeTypeLockedRoot,
		SvnNodeTypeFolder
	};

	SvnNodeType m_type;
	wxString    m_filepath;

public:
	SvnTreeData(SvnNodeType type, const wxString &filepath)
			: m_type(type)
			, m_filepath(filepath) {
	}

	virtual ~SvnTreeData() {
	}

	void SetType(const SvnNodeType& type) {
		this->m_type = type;
	}
	const SvnNodeType& GetType() const {
		return m_type;
	}
	void SetFilepath(const wxString& filepath) {
		this->m_filepath = filepath;
	}
	const wxString& GetFilepath() const {
		return m_filepath;
	}
};

#endif // SVNTREEDATA_H
