//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : qmakeplugindata.h
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

#ifndef __qmakeplugindata__
#define __qmakeplugindata__

#include <wx/string.h>
#include <map>

class QmakePluginData
{
public:
	class BuildConfPluginData
	{
	public:
		bool          m_enabled;
		wxString      m_buildConfName;
		wxString      m_qmakeConfig;
		wxString      m_qmakeExecutionLine;
		wxString      m_freeText;

	public:
		BuildConfPluginData() : m_enabled (false), m_qmakeExecutionLine(wxT("$(QMAKE)")) {}

	};

private:
	std::map<wxString, BuildConfPluginData> m_pluginsData;

public:
	QmakePluginData(const wxString &data);
	~QmakePluginData() = default;

	wxString   ToString();
	bool       GetDataForBuildConf(const wxString &configName, BuildConfPluginData &bcpd    );
	void       SetDataForBuildConf(const wxString &configName, const BuildConfPluginData &cd);
};
#endif // __qmakeplugindata__
