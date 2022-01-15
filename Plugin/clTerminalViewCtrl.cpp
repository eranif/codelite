#include "clTerminalViewCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "clAnsiEscapeCodeHandler.hpp"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "file_logger.h"

namespace
{
class MyAnsiCodeRenderer : public clControlWithItemsRowRenderer
{
    clAnsiEscapeCodeHandler handler;
    wxFont m_font;
    clDataViewListCtrl* m_ctrl = nullptr;

public:
    MyAnsiCodeRenderer(clDataViewListCtrl* ctrl)
        : m_ctrl(ctrl)
    {
    }

    void SetFont(const wxFont& f) { this->m_font = f; }
    void Render(wxWindow* window, wxDC& dc, const clColours& colours, int row_index, clRowEntry* entry) override
    {
        wxUnusedVar(window);
        wxUnusedVar(row_index);
        bool zebraColouring = (m_ctrl->HasStyle(wxTR_ROW_LINES) || m_ctrl->HasStyle(wxDV_ROW_LINES));
        bool even_row = ((row_index % 2) == 0);

        // draw the ascii line
        handler.Reset();
        handler.Parse(entry->GetLabel(0));

        if(entry->IsSelected()) {
            dc.SetPen(colours.GetSelItemBgColour());
            dc.SetBrush(colours.GetSelItemBgColour());
            dc.DrawRectangle(entry->GetItemRect());
        } else if(zebraColouring) {
            wxColour bg_colour = even_row ? colours.GetAlternateColour() : colours.GetItemBgColour();
            dc.SetPen(bg_colour);
            dc.SetBrush(bg_colour);
            dc.DrawRectangle(entry->GetItemRect());
        }

        clRenderDefaultStyle ds;
        ds.bg_colour = colours.GetBgColour();
        ds.fg_colour = colours.GetItemTextColour();
        ds.font = m_font;
        handler.Render(dc, ds, 0, entry->GetItemRect(), colours.IsLightTheme());
    }
};
} // namespace

clTerminalViewCtrl::clTerminalViewCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                       long style)
    : clDataViewListCtrl(parent, id, pos, size, style)
{
    SetRendererType(eRendererType::RENDERER_DIRECT2D);
    SetLineSpacing(0);

    SetSortFunction(nullptr);
    m_renderer = new MyAnsiCodeRenderer(this);
    SetCustomRenderer(m_renderer);
    AppendIconTextColumn(_("Message"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clTerminalViewCtrl::OnSysColourChanged, this);
    ApplyStyle();
}

clTerminalViewCtrl::~clTerminalViewCtrl()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clTerminalViewCtrl::OnSysColourChanged, this);
}

void clTerminalViewCtrl::OnSysColourChanged(clCommandEvent& e)
{
    e.Skip();
    ApplyStyle();
    Refresh();
}

void clTerminalViewCtrl::ApplyStyle()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        MyAnsiCodeRenderer* r = static_cast<MyAnsiCodeRenderer*>(m_renderer);
        wxFont f = m_rendererFont;
        if(!f.IsOk()) {
            f = lexer->GetFontForSyle(0, this);
        }
        r->SetFont(f);
        clDataViewListCtrl::SetDefaultFont(f);

        clColours colours;
        colours.InitFromColour(lexer->GetProperty(0).GetBgColour());
        colours.SetItemTextColour(lexer->GetProperty(0).GetFgColour());
        SetColours(colours);
    }
}

void clTerminalViewCtrl::AddLine(const wxString& text, bool text_ends_with_cr, wxUIntPtr data)
{
    if(IsEmpty()) {
        m_overwriteLastLine = false;
    }

    // if we need to overwrite the last item, delete the last item and
    // then call append
    if(m_overwriteLastLine) {
        DeleteItem(GetItemCount() - 1);
        m_overwriteLastLine = false;
    }

    AppendItem(text, wxNOT_FOUND, wxNOT_FOUND, data);
    if(m_scroll_to_bottom) {
        ScrollToBottom();
    }
    m_overwriteLastLine = text_ends_with_cr;
}

clAnsiEscapeCodeColourBuilder& clTerminalViewCtrl::GetBuilder(bool clear_it)
{
    m_builder.SetTheme(GetColours().IsLightTheme() ? eAsciiTheme::LIGHT : eAsciiTheme::DARK);
    if(clear_it) {
        m_builder.Clear();
    }
    return m_builder;
}

void clTerminalViewCtrl::SetDefaultFont(const wxFont& font)
{
    m_rendererFont = font;
    ApplyStyle();
}
