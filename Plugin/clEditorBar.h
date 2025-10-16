#ifndef CLEDITORBAR_H
#define CLEDITORBAR_H

#include "LSP/basic_types.h"
#include "cl_command_event.h"
#include "wxcrafter_plugin.h"

#include <optional>
#include <vector>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clEditorBar : public clEditorBarBase
{
public:
    struct ScopeEntry {
        wxString display_string;
        int line_number = wxNOT_FOUND;
        typedef std::vector<ScopeEntry> vec_t;
        LSP::Range range;
        bool operator<(const ScopeEntry& right) const { return line_number < right.line_number; }
        bool is_ok() const { return !display_string.empty() && line_number != wxNOT_FOUND; }

        inline std::optional<std::pair<int, int>> GetScopeRange(wxStyledTextCtrl* ctrl) const
        {
            if (ctrl == nullptr || !range.IsOk()) {
                return std::nullopt;
            }
            int start_pos = ctrl->PositionFromLine(range.GetStart().GetLine());
            start_pos += range.GetStart().GetCharacter();

            int end_pos = ctrl->PositionFromLine(range.GetEnd().GetLine());
            end_pos += range.GetEnd().GetCharacter();
            return std::make_pair(start_pos, end_pos);
        }
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

    bool m_shouldShow = false;

private:
    void DoRefreshColoursAndFonts();
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
    /**
     * @brief Retrieves the text of the current function in the active editor.
     *
     * This function attempts to find the function scope that contains the current line
     * in the active editor and returns the text of that function. If no active editor
     * is found, or if the current line does not belong to a valid function scope,
     * an empty optional is returned.
     *
     * @return std::optional<wxString> The text of the current function if found,
     *         otherwise std::nullopt.
     */
    std::optional<wxString> GetCurrentFunctionText() const;

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
