#ifndef CLEDITORBAR_H
#define CLEDITORBAR_H

#include "drawingutils.h"
#include "wxcrafter_plugin.h"
#include <vector>
#include <wx/colour.h>
#include <wx/font.h>

class WXDLLIMPEXP_SDK clEditorBar : public clEditorBarBase
{
    wxString m_classname;
    wxString m_function;
    wxFont m_textFont;
    wxRect m_scopeRect;
    wxBitmap m_functionBmp;

    // Breadcrumbs
    wxString m_filename;
    wxString m_filenameRelative;
    wxString m_projectFile;
    wxString m_projectName;
    wxArrayString m_breadcrumbs;

    wxRect m_filenameRect;

    // Bookmarks button
    wxBitmap m_bookmarksBmp;
    wxRect m_bookmarksRect;
    std::vector<std::pair<int, wxString> > m_bookmarks;

    bool m_shouldShow = false;

private:
    void DoRefreshColoursAndFonts();
    void DoRefresh();
    void CreateBookmarksBitmap();

public:
    clEditorBar(wxWindow* parent);
    virtual ~clEditorBar();
    void SetMessage(const wxString& className, const wxString& function);
    void DoShow(bool s);

protected:
    virtual void OnButtonActions(wxCommandEvent& event);
    virtual void OnButtonBookmarks(wxCommandEvent& event);
    virtual void OnButtonScope(wxCommandEvent& event);
    void OnMarkerChanged(clCommandEvent& event);
    void OnEditorChanged(wxCommandEvent& e);
    void OnThemeChanged(wxCommandEvent& e);
};
#endif // CLEDITORBAR_H
