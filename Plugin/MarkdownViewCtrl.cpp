#include "MarkdownViewCtrl.hpp"

#include "globals.h"

#include <algorithm>

MarkdownViewCtrl::MarkdownViewCtrl(wxWindow* parent)
    : wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_3DSASH)
{
    Hide();
    // No border: the control is inside a tab
    m_text = new clMarkdownTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_READONLY);
    m_contents = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_SINGLE | wxDV_ROW_LINES);
    m_contents->SetCursor(wxCURSOR_HAND);
    m_contents->AppendTextColumn(_("Table of Contents"), wxDATAVIEW_CELL_INERT, 100, wxALIGN_LEFT);
    // The native look: the colours of the system (and of the theme of the application), not the ones of the document
    ::AdjustDataViewAlternateColour(m_contents);
    m_renderer = std::make_unique<clRichTextMarkdownRenderer>(m_text);

    SetMinimumPaneSize(FromDIP(250));
    // The table of contents keeps its width when the window is resized
    SetSashGravity(1.0);
    SplitVertically(m_text, m_contents);
    Unsplit(m_contents);

    // A click, or Enter, on a heading
    m_contents->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &MarkdownViewCtrl::OnContentsSelected, this);
    m_contents->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MarkdownViewCtrl::OnContentsSelected, this);
    m_contents->Bind(wxEVT_SIZE, &MarkdownViewCtrl::OnContentsSize, this);
    Bind(wxEVT_SIZE, &MarkdownViewCtrl::OnSize, this);
}

bool MarkdownViewCtrl::Render(const wxString& markdown,
                              const clRichTextMarkdownRenderer::Theme& theme,
                              const wxString& baseDir)
{
    m_renderer->SetTheme(theme);
    m_renderer->SetBaseDir(baseDir);
    const bool ok = m_renderer->Render(markdown);
    FillContents();
    return ok;
}

bool MarkdownViewCtrl::GoToAnchor(const wxString& anchor) { return m_renderer->GoToAnchor(anchor); }

void MarkdownViewCtrl::FillContents()
{
    m_contents->Freeze();
    m_contents->DeleteAllItems();
    for (const auto& heading : m_renderer->GetHeadings()) {
        // The level is an indent. The row number is the index in the list of the headings
        wxVector<wxVariant> row;
        row.push_back(wxVariant(wxString(static_cast<size_t>(std::max(0, heading.level - 1)) * 3, ' ') + heading.text));
        m_contents->AppendItem(row);
    }
    m_contents->Thaw();
    m_contents->Refresh();
}

void MarkdownViewCtrl::OnContentsSelected(wxDataViewEvent& event)
{
    event.Skip();
    const int row = m_contents->ItemToRow(event.GetItem());
    const auto& headings = m_renderer->GetHeadings();
    if (row == wxNOT_FOUND || static_cast<size_t>(row) >= headings.size()) {
        return;
    }
    m_renderer->GoToAnchor(headings[row].anchor);
}

void MarkdownViewCtrl::OnContentsSize(wxSizeEvent& event)
{
    event.Skip();
    // One column: it is as wide as the control
    if (m_contents->GetColumnCount() > 0) {
        m_contents->GetColumn(0)->SetWidth(std::max(1, event.GetSize().GetWidth() - FromDIP(4)));
    }
}

void MarkdownViewCtrl::OnSize(wxSizeEvent& event)
{
    event.Skip();
    // The first time that we have a size: the table of contents gets its width
    const int width = event.GetSize().GetWidth();
    if (m_sashInitialised || width < FromDIP(400)) {
        return;
    }
    m_sashInitialised = true;
    SetSashPosition(width - FromDIP(240));
}

void MarkdownViewCtrl::ShowContents(bool show)
{
    if (show == IsContentsShown()) {
        return;
    }

    if (show) {
        // The table of contents has its width at the right side (the gravity keeps it when the window is resized). When
        // the window has no size yet (it is not shown), the splitter splits it in half
        const int width = GetClientSize().GetWidth();
        m_contents->Show();
        SplitVertically(m_text, m_contents, width > FromDIP(600) ? width - FromDIP(300) : 0);
    } else {
        // Hides the window
        Unsplit(m_contents);
    }
}
