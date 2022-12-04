#include "clTerminalViewCtrl.hpp"

#include "ColoursAndFontsManager.h"
#include "clAnsiEscapeCodeHandler.hpp"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "file_logger.h"

wxDEFINE_EVENT(wxEVT_TERMINALVIEWCTRL_LINE_ADDED, clCommandEvent);

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
        wxUnusedVar(m_ctrl);
    }

    void SetFont(const wxFont& f) { this->m_font = f; }

    /// just the draw the item background
    void RenderBackground(wxDC& dc, long tree_style, const clColours& colours, int row_index,
                          clRowEntry* entry) override
    {
        wxUnusedVar(dc);
        wxUnusedVar(tree_style);
        wxUnusedVar(colours);
        wxUnusedVar(row_index);
        wxUnusedVar(entry);
    }

    /// render item background and all text and stuff
    void Render(wxWindow* window, wxDC& dc, const clColours& colours, int row_index, clRowEntry* entry) override
    {
        wxUnusedVar(window);

        // draw the ascii line
        handler.Reset();
        handler.Parse(entry->GetLabel(0));

        dc.SetBrush(colours.GetItemBgColour());
        dc.SetPen(colours.GetItemBgColour());
        dc.DrawRectangle(entry->GetItemRect());

        if(entry->IsSelected()) {
            dc.SetPen(colours.GetSelItemBgColour());
            dc.SetBrush(colours.GetSelItemBgColour());
            dc.DrawRectangle(entry->GetItemRect());
        }

        clRenderDefaultStyle ds;
        ds.bg_colour = colours.GetItemBgColour();
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
            f = lexer->GetFontForStyle(0, this);
        }

        r->SetFont(f);
        clDataViewListCtrl::SetDefaultFont(f);

        // construct colours based on the current lexer
        clColours colours;
        colours.FromLexer(lexer);
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

    clCommandEvent event_line_added{ wxEVT_TERMINALVIEWCTRL_LINE_ADDED };
    GetEventHandler()->AddPendingEvent(event_line_added);
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
