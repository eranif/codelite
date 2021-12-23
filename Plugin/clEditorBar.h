#ifndef CLEDITORBAR_H
#define CLEDITORBAR_H

#include "cl_command_event.h"
#include "drawingutils.h"
#include "wxcrafter_plugin.h"

#include <vector>
#include <wx/colour.h>
#include <wx/font.h>

class WXDLLIMPEXP_SDK clEditorBar : public clEditorBarBase
{
public:
    struct ScopeEntry {
        wxString display_string;
        int line_number = wxNOT_FOUND;
        typedef std::vector<ScopeEntry> vec_t;
        bool operator<(const ScopeEntry& right) const { return line_number < right.line_number; }
        bool is_ok() const { return !display_string.empty() && line_number != wxNOT_FOUND; }
    };

private:
    ScopeEntry::vec_t m_scopes;
    wxString m_scopesFile;
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
    std::vector<std::pair<int, wxString>> m_bookmarks;

    bool m_shouldShow = false;

private:
    void DoRefreshColoursAndFonts();
    void DoRefresh();
    void CreateBookmarksBitmap();
    const clEditorBar::ScopeEntry& FindByLine(int lineNumber) const;
    void UpdateScope();

public:
    clEditorBar(wxWindow* parent);
    virtual ~clEditorBar();
    void SetScopes(const wxString& filename, const clEditorBar::ScopeEntry::vec_t& entries);
    bool ShouldShow() const { return m_shouldShow; }
    void DoShow(bool s);
    void SetLabel(const wxString& text);
    void ClearLabel() { SetLabel(wxEmptyString); }
    wxString GetLabel() const;

protected:
    virtual void OnButtonActions(wxCommandEvent& event);
    virtual void OnButtonBookmarks(wxCommandEvent& event);
    virtual void OnButtonScope(wxCommandEvent& event);
    void OnMarkerChanged(clCommandEvent& event);
    void OnEditorChanged(wxCommandEvent& e);
    void OnThemeChanged(clCommandEvent& e);
    void OnUpdate(clCodeCompletionEvent& event);
};
#endif // CLEDITORBAR_H
