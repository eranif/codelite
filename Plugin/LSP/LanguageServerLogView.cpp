#include "LanguageServerLogView.h"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "codelite_events.h"
#include "event_notifier.h"

#include <algorithm>
#include <wx/dcclient.h>
#include <wx/menu.h>

namespace
{
/// A progress bar renderer that uses the IDE colours (the native one is white on white in some themes)
class ProgressGaugeRenderer : public wxDataViewCustomRenderer
{
public:
    ProgressGaugeRenderer()
        : wxDataViewCustomRenderer("long", wxDATAVIEW_CELL_INERT, wxALIGN_CENTER_VERTICAL)
    {
    }

    bool SetValue(const wxVariant& value) override
    {
        m_percentage = std::clamp(value.GetLong(), 0L, 100L);
        return true;
    }

    bool GetValue(wxVariant& value) const override
    {
        value = m_percentage;
        return true;
    }

    // A negative size makes wxWidgets pass the whole cell to Render() (it shrinks the cell to GetSize() otherwise).
    // The row height is taken from the text columns.
    wxSize GetSize() const override { return wxSize(-1, -1); }

    bool Render(wxRect rect, wxDC* dc, int state) override
    {
        wxUnusedVar(state);
        rect.Deflate(2);

        const wxColour bg = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
        const wxColour border = clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
        const wxColour fill = clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);

        dc->SetPen(border);
        dc->SetBrush(bg);
        dc->DrawRectangle(rect);

        wxRect fillRect = rect;
        fillRect.Deflate(1);
        fillRect.SetWidth(fillRect.GetWidth() * m_percentage / 100);
        if (fillRect.GetWidth() > 0) {
            dc->SetPen(fill);
            dc->SetBrush(fill);
            dc->DrawRectangle(fillRect);
        }

        // draw the text twice: once for the empty part and once (clipped) for the filled part
        const wxString text = wxString::Format("%ld%%", m_percentage);
        const wxSize textSize = dc->GetTextExtent(text);
        const wxPoint pt(rect.x + (rect.width - textSize.x) / 2, rect.y + (rect.height - textSize.y) / 2);
        dc->SetTextForeground(clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        dc->DrawText(text, pt);
        if (fillRect.GetWidth() > 0) {
            wxDCClipper clipper(*dc, fillRect);
            dc->SetTextForeground(clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
            dc->DrawText(text, pt);
        }
        return true;
    }

private:
    long m_percentage{0};
};
} // namespace

LanguageServerLogView::LanguageServerLogView(wxWindow* parent)
    : LanguageServerLogViewBase(parent)
{
    m_stcLog->SetReadOnly(true);

    GetDvListCtrlProgress()->AppendTextColumn(_("Server"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(120));
    GetDvListCtrlProgress()->AppendColumn(
        new wxDataViewColumn(_("Progress"), new ProgressGaugeRenderer(), 1, WXC_FROM_DIP(150), wxALIGN_LEFT),
        "long");
    GetDvListCtrlProgress()->AppendTextColumn(
        _("Message"), wxDATAVIEW_CELL_INERT, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

    m_stcLog->Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event) {
        wxMenu menu;
        menu.Append(wxID_CLEAR);
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& event) {
                wxUnusedVar(event);
                // clear the view
                m_stcLog->SetReadOnly(false);
                m_stcLog->ClearAll();
                m_stcLog->SetReadOnly(true);
            },
            wxID_CLEAR);
        m_stcLog->PopupMenu(&menu);
    });
    DoColourChanged();
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerLogView::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &LanguageServerLogView::OnColoursChanged, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_PROGRESS, &LanguageServerLogView::OnProgress, this);
}

LanguageServerLogView::~LanguageServerLogView()
{
    EventNotifier::Get()->Unbind(wxEVT_LSP_PROGRESS, &LanguageServerLogView::OnProgress, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &LanguageServerLogView::OnColoursChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerLogView::OnWorkspaceClosed, this);
}

void LanguageServerLogView::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    m_stcLog->SetReadOnly(false);
    m_stcLog->ClearAll();
    m_stcLog->SetReadOnly(true);
    ClearProgress();
}

void LanguageServerLogView::ClearProgress()
{
    GetDvListCtrlProgress()->DeleteAllItems();
    m_progressKeys.clear();
}

void LanguageServerLogView::OnProgress(LSPEvent& event)
{
    event.Skip();

    const auto& progress = event.GetProgress();
    auto list = GetDvListCtrlProgress();
    const wxString key = event.GetServerName() + "\n" + progress.m_token;
    const auto where = std::find(m_progressKeys.begin(), m_progressKeys.end(), key);

    if (progress.m_kind == LSP::ProgressKind::end) {
        if (where != m_progressKeys.end()) {
            list->DeleteItem(std::distance(m_progressKeys.begin(), where));
            m_progressKeys.erase(where);
        }
        return;
    }

    const long percentage = std::clamp(static_cast<long>(progress.m_percentage), 0L, 100L);
    if (where == m_progressKeys.end()) {
        // `begin`, or a `report` without a `begin`
        wxVector<wxVariant> row;
        row.push_back(event.GetServerName());
        row.push_back(percentage);
        row.push_back(progress.m_message);
        list->AppendItem(row);
        m_progressKeys.push_back(key);
    } else {
        const unsigned row = std::distance(m_progressKeys.begin(), where);
        if (!progress.m_message.empty()) {
            list->SetTextValue(progress.m_message, row, 2);
        }
        list->SetValue(wxVariant(percentage), row, 1);
    }
}

void LanguageServerLogView::OnColoursChanged(clCommandEvent& event)
{
    event.Skip();
    DoColourChanged();
}

void LanguageServerLogView::DoColourChanged()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    CHECK_PTR_RET(lexer);

    lexer->Apply(m_stcLog);
}