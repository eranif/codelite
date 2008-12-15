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

#include <map>
#include <vector>
#include "wx/wxscintilla.h"

#include "custom_notebook.h"
#include "outputtabwindow.h"


class FindResultsTab : public OutputTabWindow
{
private:
    struct fifMatchInfo {
        fifMatchInfo(const wxString &locationInfoStr, const wxString &fileName);

        wxString file_name;
        long     line_number;
        long     match_len;
        long     col;
    };

    Notebook    *m_book; // for multiple Find Results pages
    wxScintilla *m_recv; // the page that is receiving results of a search

	std::vector<std::map<int, fifMatchInfo> > m_matchInfo;

	void AppendText(const wxString &line, bool displayScope);
	void Clear();

    void OnSearchStart   (wxCommandEvent   &e);
    void OnSearchMatch   (wxCommandEvent   &e);
    void OnSearchEnded   (wxCommandEvent   &e);
    void OnSearchCancel  (wxCommandEvent   &e);
    void OnPageChanged   (NotebookEvent    &e);
	void OnMouseDClick   (wxScintillaEvent &e);
	void OnHotspotClicked(wxScintillaEvent &e);
	void OnMarginClick   (wxScintillaEvent &e);
	void OnCollapseAll   (wxCommandEvent   &e);

public:
	FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name);
	~FindResultsTab();
};
#endif // __findresultstab__
