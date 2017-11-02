#include "clEditorBar.h"
#include <wx/dcmemory.h>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include "globals.h"
#include "imanager.h"
#include "ieditor.h"
#include <wx/settings.h>
#include "lexer_configuration.h"
#include "ColoursAndFontsManager.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "drawingutils.h"

clEditorBar::clEditorBar(wxWindow* parent)
    : clEditorBarBase(parent)
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    m_defaultColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_functionColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_classColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    m_textFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    LexerConf::Ptr_t defaultLexer = ColoursAndFontsManager::Get().GetLexer("default");
    if(defaultLexer) {
        m_textFont = defaultLexer->GetFontForSyle(0);
    }

    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clEditorBar::OnThemeChanged, this);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    memDC.SetFont(m_textFont);
    wxSize sz = memDC.GetTextExtent("Tp");
    sz.y += 6; // 2*3 pixels
    SetSizeHints(wxSize(-1, sz.y));
}

clEditorBar::~clEditorBar()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_PAGE_CHANGED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &clEditorBar::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clEditorBar::OnThemeChanged, this);
}

void clEditorBar::OnEraseBG(wxEraseEvent& event) { wxUnusedVar(event); }
void clEditorBar::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC bdc(this);
    PrepareDC(bdc);
    wxDC& dc = bdc;
    wxRect rect(GetClientRect());
    dc.SetPen(m_bgColour);
    dc.SetBrush(m_bgColour);
    dc.DrawRectangle(rect);
    dc.SetFont(m_textFont);

    wxString fulltext;
    if(!m_classname.IsEmpty()) {
        fulltext << m_classname << "::";
    }
    if(!m_function.IsEmpty()) {
        fulltext << m_function;
    }

    // Draw the text
    wxSize textSize;
    wxCoord textX = 10;
    wxCoord textY = ((rect.GetHeight() - dc.GetTextExtent("Tp").GetY()) / 2);
    if(!m_classname.IsEmpty()) {
        textSize = dc.GetTextExtent(m_classname);
        dc.SetTextForeground(m_classColour);
        dc.DrawText(m_classname, textX, textY);
        textX += textSize.x;

        textSize = dc.GetTextExtent("::");
        dc.SetTextForeground(m_defaultColour);
        dc.DrawText("::", textX, textY);
        textX += textSize.x;
    }

    if(!m_function.IsEmpty()) {
        textSize = dc.GetTextExtent(m_function);
        dc.SetTextForeground(m_functionColour);
        dc.DrawText(m_function, textX, textY);
        textX += textSize.x;
    }
}

void clEditorBar::OnEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    DoRefreshColoursAndFonts();
}

void clEditorBar::SetMessage(const wxString& className, const wxString& function)
{
    if((className != m_classname) || (function != m_function)) {
        m_classname = className;
        m_function = function;
        Refresh();
    }
}

void clEditorBar::DoShow(bool s)
{
    if(Show(s)) {
        GetParent()->GetSizer()->Layout();
    }
}

void clEditorBar::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    DoRefreshColoursAndFonts();
}

void clEditorBar::DoRefreshColoursAndFonts()
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
        if(lexer) {
            wxColour m_bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
            bool darkBG = DrawingUtils::IsDark(m_bgColour);
            m_defaultColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
            m_classColour = darkBG ? "rgb(224, 108, 117)" : "rgb(0, 64, 128)";
            m_functionColour = m_defaultColour;
            m_textFont = lexer->GetFontForSyle(0); // Default font
            m_textFont.SetPointSize(m_textFont.GetPointSize() - 2);
        }
    } else {
        m_classname.clear();
        m_function.clear();
        m_bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    }
    Refresh();
}

void clEditorBar::OnEditorSize(wxSizeEvent& event)
{
    event.Skip();
    DoRefreshColoursAndFonts();
}
