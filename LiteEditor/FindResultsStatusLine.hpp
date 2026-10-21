#pragma once

#include "search_thread.h"

#include <wx/panel.h>
#include <wx/stattext.h>

/// The status line of the "Find In Files" tabs. It describes the search, and when the search ends, it shows its
/// summary. It uses the system colours (not the lexer ones) and draws a border around itself
class FindResultsStatusLine : public wxPanel
{
public:
    explicit FindResultsStatusLine(wxWindow* parent);
    ~FindResultsStatusLine() override = default;

    /// Remove the text
    void Clear();
    /// A search for `data` is running
    void ShowSearching(const SearchData& data);
    /// The search for `data` ended
    void ShowSummary(const SearchSummary& summary, const SearchData& data);
    void ShowCancelled();
    /// The results of an earlier search for `data` (`matches` matches) are displayed
    void ShowHistory(const SearchData& data, size_t matches);

    /// Use the system colours, and `font`
    void ApplyTheme(const wxFont& font);

private:
    /// Set the text. The tooltip defaults to the text
    void SetStatus(const wxString& text, const wxString& tooltip = wxEmptyString);
    void OnPaint(wxPaintEvent& event);

    wxString m_text;
    wxFont m_statusFont;
};
