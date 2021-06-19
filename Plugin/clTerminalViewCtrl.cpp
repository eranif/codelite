#include "ColoursAndFontsManager.h"
#include "clAsciiEscapCodeHandler.hpp"
#include "clSystemSettings.h"
#include "clTerminalViewCtrl.hpp"
#include "event_notifier.h"

namespace
{
class MyAsciiRenderer : public clControlWithItemsRowRenderer
{
    clAsciiEscapeCodeHandler handler;
    wxFont m_font;

public:
    MyAsciiRenderer() {}

    void SetFont(const wxFont& f) { this->m_font = f; }
    void Render(wxWindow* window, wxDC& dc, const clColours& colours, int row_index, clRowEntry* entry) override
    {
        wxUnusedVar(window);
        wxUnusedVar(row_index);

        // draw the ascii line
        handler.Reset();
        handler.Parse(entry->GetLabel(0));

        if(entry->IsSelected()) {
            dc.SetPen(colours.GetSelItemBgColour());
            dc.SetBrush(colours.GetSelItemBgColour());
            dc.DrawRectangle(entry->GetItemRect());
        }

        clRenderDefaultStyle ds;
        ds.bg_colour = colours.GetBgColour();
        ds.fg_colour = colours.GetItemTextColour();
        ds.font = m_font;
        handler.Render(dc, ds, 0, entry->GetItemRect());
    }
};
} // namespace

clTerminalViewCtrl::clTerminalViewCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                       long style)
    : clDataViewListCtrl(parent, id, pos, size, style)
{
    m_renderer = new MyAsciiRenderer();
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
        MyAsciiRenderer* r = static_cast<MyAsciiRenderer*>(m_renderer);
        wxFont f = lexer->GetFontForSyle(0, this);
        r->SetFont(f);
        clColours colours;
        colours.InitFromColour(lexer->GetProperty(0).GetBgColour());
        colours.SetItemTextColour(lexer->GetProperty(0).GetFgColour());
        SetDefaultFont(f);
        SetColours(colours);
    }
}
