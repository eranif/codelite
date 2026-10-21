#include "FindResultsStatusLine.hpp"

#include "ColoursAndFontsManager.h"
#include "UnicodeSymbols.hpp"
#include "drawingutils.h"

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
    return wxString::Format(wxT(" ❘ Match-Case: %s ❘ Whole-Word: %s ❘ Regex: %s"),
                            mark(data.IsMatchCase()),
                            mark(data.IsMatchWholeWord()),
                            mark(data.IsRegularExpression()));
}
} // namespace

FindResultsStatusLine::FindResultsStatusLine(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE | wxBORDER_NONE)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &FindResultsStatusLine::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
    Bind(wxEVT_SIZE, [this](wxSizeEvent& e) {
        e.Skip();
        Refresh();
    });

    const auto font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

#ifdef __WXMSW__
    m_statusFont = wxFont(
        font.GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Segoe UI Emoji");
#elif defined(__WXMSW__)
    m_statusFont = wxFont(
        font.GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Noto Color Emoji");
#else
    m_statusFont = font;
#endif

    int height{0};
    GetTextExtent(_("Tp"), nullptr, &height, nullptr, nullptr, &m_statusFont);
    SetSizeHints(wxNOT_FOUND, height + FromDIP(2));
}

void FindResultsStatusLine::SetStatus(const wxString& text, const wxString& tooltip)
{
    m_text = text;
    SetToolTip(tooltip.empty() ? text : tooltip);
    Refresh();
}

void FindResultsStatusLine::Clear() { SetStatus(wxEmptyString); }

void FindResultsStatusLine::ShowSearching(const SearchData& data)
{
    SetStatus(wxString::Format(_("%s Searching for '%s'..."), SearchIcon(), data.GetFindString()) +
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
    message << FormatSearchOptions(data);

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
    wxUnusedVar(font);
    // The face of the results font, but with the size of the default GUI font (a fractional point size)
    Layout();
    Refresh();
}

void FindResultsStatusLine::OnPaint(wxPaintEvent& event)
{
    wxUnusedVar(event);
    const wxRect rect = GetClientRect();

    const wxColour bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    const bool isDark = DrawingUtils::IsDark(bgColour);
    const wxColour bottomColour = isDark ? *wxBLACK : wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
    const wxColour topColour = isDark ? wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT) : *wxWHITE;
    const wxColour textColour = bgColour.ChangeLightness(isDark ? 170 : 30);

    // A graphics context draws colour emojis (Direct2D on Windows). It falls back to the plain DC
    wxPaintDC paintDC{this};
    wxGCDC gcdc;
    wxDC& dc = DrawingUtils::GetGCDC(paintDC, gcdc);

    dc.SetBrush(bgColour);
    dc.SetPen(bgColour);
    dc.DrawRectangle(rect);

    dc.SetPen(bottomColour);
    dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());

    dc.SetPen(topColour);
    dc.DrawLine(rect.GetTopLeft(), rect.GetTopRight());
    if (m_text.empty())
        return;

    dc.SetFont(m_statusFont);
    wxRect textRect = dc.GetTextExtent(m_text);
    textRect = textRect.CentreIn(rect);
    dc.SetTextForeground(textColour);
    dc.DrawText(m_text, textRect.GetTopLeft());
}
