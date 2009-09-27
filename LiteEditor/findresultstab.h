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
#include <list>
#include "wx/wxscintilla.h"

#include "custom_notebook.h"
#include "outputtabwindow.h"
#include "search_thread.h"
#include "findinfilesdlg.h"

// Map between the line numbers and a search results
typedef std::map<int, SearchResult> MatchInfo;

// List containing the match info for all tabs (pre location)
typedef std::list<MatchInfo> ListMatchInfos;

class FindResultsTab : public OutputTabWindow
{
	SearchData m_searchData;
	bool       m_searchInProgress;
protected:
    static FindInFilesDialog *m_find;

    Notebook    *m_book; // for multiple Find Results pages
    wxScintilla *m_recv; // the page that is receiving results of a search

	ListMatchInfos m_matchInfo;

	MatchInfo& GetMatchInfo(size_t idx = 0);

	void AppendText(const wxString &line);
	void Clear();

	virtual void OnPageClosed      (NotebookEvent    &e);
    virtual void OnPageChanged     (NotebookEvent    &e);
    virtual void OnFindInFiles     (wxCommandEvent   &e);
    virtual void OnSearchStart     (wxCommandEvent   &e);
    virtual void OnSearchMatch     (wxCommandEvent   &e);
    virtual void OnSearchEnded     (wxCommandEvent   &e);
    virtual void OnSearchCancel    (wxCommandEvent   &e);
    virtual void OnClearAll        (wxCommandEvent   &e);
    virtual void OnRepeatOutput    (wxCommandEvent   &e);

	virtual void OnCloseTab        (wxCommandEvent   &e);
	virtual void OnCloseAllTabs    (wxCommandEvent   &e);
	virtual void OnCloseOtherTab   (wxCommandEvent   &e);
	virtual void OnTabMenuUI       (wxUpdateUIEvent  &e);

	virtual void OnClearAllUI      (wxUpdateUIEvent  &e);
	virtual void OnRepeatOutputUI  (wxUpdateUIEvent  &e);
	virtual void OnMouseDClick     (wxScintillaEvent &e);
	SearchData   GetSearchData     (wxScintilla *sci   );
	void         DoOpenSearchResult(const SearchResult &result, wxScintilla *sci, int markerLine);
    DECLARE_EVENT_TABLE()

public:
	FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name, bool useBook = false);
	~FindResultsTab();

    void        LoadFindInFilesData();
    void        SaveFindInFilesData();
	long        GetBookStyle();
    static void SetStyles(wxScintilla *sci);
	size_t      GetPageCount() const ;
	void        NextMatch();
	void        PrevMatch();
};
#endif // __findresultstab__
