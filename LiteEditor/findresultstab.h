//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findresultstab.h
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
 #ifndef __findresultstab__
#define __findresultstab__

#include "outputtabwindow.h"
#include <vector>

class fifMatchInfo {
public:
	wxString file_name;
	long line_number;
	long match_len;
	long col;

public:
	void FromString(const wxString &locationInfoStr, const wxString &fileName);
	fifMatchInfo();
	fifMatchInfo(const wxString &locationInfoStr, const wxString &fileName);
	fifMatchInfo(const wxString &fileName);
};

class FindResultsTab : public OutputTabWindow
{
	std::vector<fifMatchInfo> m_matchInfo;

private:
	FindResultsTab(const FindResultsTab& rhs);
	FindResultsTab& operator=(const FindResultsTab& rhs);
	void DefineMarker(int marker, int markerType, wxColor fore, wxColor back);
	void OnMarginClick(wxScintillaEvent& event);
	void DoMatchClicked(long pos);
public:
	FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name);
	virtual ~FindResultsTab();

	virtual void OnMouseDClick(wxScintillaEvent &event);
	virtual void OnHotspotClicked(wxScintillaEvent &event);
	virtual void AppendText(const wxString &line);
	virtual void Clear();
};
#endif // __findresultstab__
