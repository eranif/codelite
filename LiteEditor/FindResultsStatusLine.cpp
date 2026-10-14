#include "FindResultsStatusLine.hpp"

#include "UnicodeSymbols.hpp"
#include "drawingutils.h"

#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/translation.h>

namespace
{
// The symbols of the status line
wxString SearchIcon() { return GetUnicodeSymbol(IconType::kSearch); }
wxString WarningIcon() { return GetUnicodeSymbol(IconType::kWarningSign); }

/// The search options, for example: "Regex: <yes>   Match-Case: <no>   Whole-Word: <no>"
wxString FormatSearchOptions(const SearchData& data)
{
    auto mark = [](bool enabled) { return GetUnicodeSymbol(enabled ? IconType::kSuccess : IconType::kError); };
    return wxString::Format(_("Regex: %s   Match-Case: %s   Whole-Word: %s"),
                            mark(data.IsRegularExpression()),
                            mark(data.IsMatchCase()),
                            mark(data.IsMatchWholeWord()));
}
} // namespace

#ifdef __WXMSW__
constexpr long kBorderStyle = wxFULL_REPAINT_ON_RESIZE | wxBORDER_SIMPLE;
#else
constexpr long kBorderStyle = wxFULL_REPAINT_ON_RESIZE | wxBORDER_NONE;
#endif

FindResultsStatusLine::FindResultsStatusLine(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, kBorderStyle)
{
    m_label = new wxStaticText(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END | wxBORDER_NONE);
    m_label->SetMinSize(wxSize(1, wxNOT_FOUND));

    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_label, wxSizerFlags(1).Expand().Border(wxALL, 5));
    SetSizer(sizer);
}

void FindResultsStatusLine::SetStatus(const wxString& text, const wxString& tooltip)
{
    m_label->SetLabel(text);
    m_label->SetToolTip(tooltip.empty() ? text : tooltip);
}

void FindResultsStatusLine::Clear() { SetStatus(wxEmptyString); }

void FindResultsStatusLine::ShowSearching(const SearchData& data)
{
    SetStatus(wxString::Format(_("%s Searching for '%s'..."), SearchIcon(), data.GetFindString()) + "   " +
              FormatSearchOptions(data));
}

void FindResultsStatusLine::ShowSummary(const SearchSummary& summary, const SearchData& data)
{
    // For example: "<magnifier> 'foo'   Files scanned: 120   Matches: 45   Time: 0.31 seconds   Regex: <yes> ..."
    wxString message;
    message << SearchIcon() << " '" << data.GetFindString() << "'";
    if (summary.GetNumFileScanned() > 0) {
        message << "   " << wxString::Format(_("Files scanned: %d"), summary.GetNumFileScanned());
    }
    message << "   " << wxString::Format(_("Matches: %d"), summary.GetNumMatchesFound());
    message << "   " << wxString::Format(_("Time: %.2f seconds"), summary.GetElapsedTime() / 1000.0);
    message << "   " << FormatSearchOptions(data);

    // The files that could not be opened are listed in the tooltip
    wxString tooltip;
    const wxArrayString& failedFiles = summary.GetFailedFiles();
    if (!failedFiles.IsEmpty()) {
        message << "   " << WarningIcon() << " "
                << wxString::Format(_("Failed to open %d files"), static_cast<int>(failedFiles.size()));
        tooltip << message << "\n\n" << _("Failed to open the following files for scan:");
        for (const wxString& file : failedFiles) {
            tooltip << "\n" << file;
        }
    }
    SetStatus(message, tooltip);
}

void FindResultsStatusLine::ShowCancelled() { SetStatus(SearchIcon() + " " + _("Search cancelled by user")); }

void FindResultsStatusLine::ShowHistory(const SearchData& data, size_t matches)
{
    SetStatus(SearchIcon() + wxString::Format(" '%s'   ", data.GetFindString()) +
              wxString::Format(_("Matches: %d"), static_cast<int>(matches)) + "   " + FormatSearchOptions(data));
}

void FindResultsStatusLine::ApplyTheme(const wxFont& font)
{
    // The face of the results font, but with the size of the default GUI font (a fractional point size)
    wxFont statusFont = font;
    statusFont.SetFractionalPointSize(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFractionalPointSize());
    m_label->SetFont(statusFont);
    Layout();
    Refresh();
}

void FindResultsStatusLine::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    wxAutoBufferedPaintDC dc(this);
    dc.SetPen(wxPen(DrawingUtils::GetThemeBorderColour()));
    dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    dc.DrawRectangle(GetClientRect());
}
