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
#include "search_thread.h"
#include "findinfilesdlg.h"


class FindResultsTab : public OutputTabWindow
{
private:
    static FindInFilesDialog *m_find;

protected:
    Notebook    *m_book; // for multiple Find Results pages
    wxScintilla *m_recv; // the page that is receiving results of a search
    SearchData   m_data;

	std::vector<std::map<int, SearchResult> > m_matchInfo;

	void AppendText(const wxString &line);
	void Clear();

    virtual void OnFindInFiles   (wxCommandEvent   &e);
    virtual void OnRepeatSearch  (wxCommandEvent   &e);
    virtual void OnSearchStart   (wxCommandEvent   &e);
    virtual void OnSearchMatch   (wxCommandEvent   &e);
    virtual void OnSearchEnded   (wxCommandEvent   &e);
    virtual void OnSearchCancel  (wxCommandEvent   &e);
    virtual void OnPageChanged   (NotebookEvent    &e);
	virtual void OnMouseDClick   (wxScintillaEvent &e);
	virtual void OnHotspotClicked(wxScintillaEvent &e);
	virtual void OnMarginClick   (wxScintillaEvent &e);
	virtual void OnCollapseAll   (wxCommandEvent   &e);

public:
	FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name, size_t numpage = 5);
	~FindResultsTab();

    void LoadFindInFilesData();
    void SaveFindInFilesData();

    static void SetStyles(wxScintilla *sci);
	size_t GetPageCount() const ;
};
#endif // __findresultstab__
