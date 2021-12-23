//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clWorkspaceView.h
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

#ifndef CLWORKSPACEVIEW_H
#define CLWORKSPACEVIEW_H

#include "clWorkspaceEvent.hpp"
#include "codelite_exports.h"

#include <map>
#include <vector>
#include <wx/simplebook.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clWorkspaceView : public wxEvtHandler
{
    wxSimplebook* m_simpleBook;
    wxString m_defaultPage;
    std::map<wxString, wxWindow*> m_windows;

protected:
    /**
     * @brief return the index of a give page by its name.
     * @param name the page name
     * @return page index or wxString::npos on failure
     */
    size_t GetPageIndex(const wxString& name) const;

    /**
     * @brief a workspace was closed
     */
    void OnWorkspaceClosed(clWorkspaceEvent& event);

public:
    clWorkspaceView(wxSimplebook* book);
    virtual ~clWorkspaceView();

    /**
     * @brief return the default page to show
     */
    const wxString& GetDefaultPage() const { return m_defaultPage; }

    /**
     * @brief set the default page to show
     */
    void SetDefaultPage(const wxString& page) { m_defaultPage = page; }

    /**
     * @brief add custom page to the inner notebook
     * @note adding a page does not select it
     */
    void AddPage(wxWindow* page, const wxString& name, bool addToBook = true);

    /**
     * @brief return page with given name
     */
    wxWindow* GetPage(const wxString& name) const;

    /**
     * @brief select the current page to display
     * @param name
     */
    void SelectPage(const wxString& name);

    /**
     * @brief remove page from the inner notebook
     * @note if page with name does not exist, do nothing
     */
    void RemovePage(const wxString& name);

    /**
     * @brief return the underlying book control mananged by the view
     */
    wxSimplebook* GetBook() { return m_simpleBook; }

    /**
     * @brief return all pages
     */
    std::map<wxString, wxWindow*> GetAllPages() const;
};

#endif // CLWORKSPACEVIEW_H
