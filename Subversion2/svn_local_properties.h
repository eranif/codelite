//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : svn_local_properties.h
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

#ifndef SUBVERSIONLOCALPROPERTIES_H
#define SUBVERSIONLOCALPROPERTIES_H

#include <wx/string.h>
#include <map>

using SimpleTable = std::map<wxString, wxString>;
using GroupTable = std::map<wxString, SimpleTable>;

class SubversionLocalProperties
{
	wxString   m_url;
	GroupTable m_values;

public:
	static wxString BUG_TRACKER_URL;
	static wxString BUG_TRACKER_MESSAGE;
	static wxString FR_TRACKER_URL;
	static wxString FR_TRACKER_MESSAGE;

protected:
	wxString GetConfigFile();
	void     ReadProperties();
	void     WriteProperties();

public:
	SubversionLocalProperties(const wxString &url);
	virtual ~SubversionLocalProperties() = default;

	wxString ReadProperty (const wxString &propName);
	void     WriteProperty(const wxString &name, const wxString &val);
};

#endif // SUBVERSIONLOCALPROPERTIES_H
