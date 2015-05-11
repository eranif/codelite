#ifndef CLWORKSPACEVIEW_H
#define CLWORKSPACEVIEW_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/simplebook.h>

class WXDLLIMPEXP_SDK clWorkspaceView
{
    wxSimplebook* m_simpleBook;

protected:
    /**
     * @brief return the index of a give page by its name.
     * @param name the page name
     * @return page index or wxString::npos on failure
     */
    size_t GetPageIndex(const wxString& name) const;

public:
    clWorkspaceView(wxSimplebook* book);
    virtual ~clWorkspaceView();

    // API for adding custom workspace trees

    /**
     * @brief add custom page to the inner notebook
     * @note adding a page does not select it
     */
    void AddPage(wxWindow* page, const wxString& name);

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
};

#endif // CLWORKSPACEVIEW_H
