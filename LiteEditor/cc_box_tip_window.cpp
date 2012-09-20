#include "cc_box_tip_window.h"
#include <wx/bitmap.h>
#include <wx/dcmemory.h>
#include "ieditor.h"
#include "pluginmanager.h"
#include "cl_editor.h"
#include <wx/settings.h>
#include <wx/dcbuffer.h>
#include "bitmap_loader.h"
#include <wx/tokenzr.h>
#include <wx/spinctrl.h>
#include "Markup.h"
#include "cc_box.h"
#include "event_notifier.h"

wxBitmap CCBoxTipWindow::m_leftbmp = wxNullBitmap;
wxBitmap CCBoxTipWindow::m_rightbmp  = wxNullBitmap;

const wxEventType wxEVT_TIP_BTN_CLICKED_UP   = wxNewEventType();
const wxEventType wxEVT_TIP_BTN_CLICKED_DOWN = wxNewEventType();

CCBoxTipWindow::CCBoxTipWindow(wxWindow* parent, const wxString &tip, size_t numOfTips)
    : wxPopupWindow(parent)
    , m_tip(tip)
    , m_numOfTips(numOfTips)
    , m_positionedToRight(true)
{
    //Invalidate the rectangles
    m_leftTipRect = wxRect();
    m_rightTipRect = wxRect();

    if ( m_leftbmp.IsNull() || m_rightbmp.IsNull() ) {
        m_leftbmp  = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("cc/16/prev-tip"));
        m_rightbmp = PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("cc/16/next-tip"));
    }

    m_tip.Trim().Trim(false);
    m_tip.Replace(wxT("\n<hr>"), wxT("<hr>"));

    if ( m_numOfTips > 1 )
        m_tip.Prepend(wxT("\n\n")); // Make room for the spinctrl

    Hide();

    wxBitmap bmp(1, 1);
    wxMemoryDC dc(bmp);

    wxSize size;
    m_commentFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    m_codeFont    = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);

    wxString codePart, commentPart;
    wxString strippedTip = DoStripMarkups();
    int where= strippedTip.Find("<hr>");
    if ( where != wxNOT_FOUND ) {
        codePart    = strippedTip.Mid(0, where);
        commentPart = strippedTip.Mid(where + 5);

    } else {
        codePart = strippedTip;
    }

    int commentWidth, codeWidth;

    // Use bold font for measurements
    m_codeFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_commentFont.SetWeight(wxFONTWEIGHT_BOLD);

    dc.GetMultiLineTextExtent(codePart,    &codeWidth,    NULL, NULL, &m_codeFont);
    dc.GetMultiLineTextExtent(commentPart, &commentWidth, NULL, NULL, &m_commentFont);

    m_codeFont.SetWeight(wxFONTWEIGHT_NORMAL);
    m_commentFont.SetWeight(wxFONTWEIGHT_NORMAL);

    // Set the width
    commentWidth > codeWidth ? size.x = commentWidth : size.x = codeWidth;

    // calculate the height
    m_lineHeight = 16;
    dc.GetTextExtent(wxT("Tp"), NULL, &m_lineHeight, NULL, NULL, &m_codeFont);
    int nLineCount = ::wxStringTokenize(m_tip, wxT("\r\n"), wxTOKEN_RET_EMPTY_ALL).GetCount();
    nLineCount++;

    size.y = nLineCount * m_lineHeight;
    size.x += 40;
    SetSize(size);

    Connect(wxEVT_PAINT, wxPaintEventHandler(CCBoxTipWindow::OnPaint), NULL, this);
    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(CCBoxTipWindow::OnEraseBG), NULL, this);
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(CCBoxTipWindow::OnMouseLeft), NULL, this);
}

CCBoxTipWindow::~CCBoxTipWindow()
{
    Disconnect(wxEVT_PAINT, wxPaintEventHandler(CCBoxTipWindow::OnPaint), NULL, this);
    Disconnect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(CCBoxTipWindow::OnEraseBG), NULL, this);
    Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(CCBoxTipWindow::OnMouseLeft), NULL, this);
}

void CCBoxTipWindow::PositionRelativeTo(wxWindow* win, LEditor* focusEdior)
{
    m_positionedToRight = true;
    // When shown, set the focus back to the editor
    wxPoint pt = win->GetScreenPosition();
    pt.x += win->GetSize().x;

    // Check for overflow
    wxSize  size = ::wxGetDisplaySize();
    if ( pt.x + GetSize().x > size.x ) {
        // Move the tip to the left
        pt = win->GetScreenPosition();
        pt.x -= GetSize().x;
        m_positionedToRight = false;

        if ( pt.x < 0 ) {
            // the window can not fit to the screen, restore its position
            // to the right side
            pt = win->GetScreenPosition();
            pt.x += win->GetSize().x;
            m_positionedToRight = true;
        }
    }

    if ( focusEdior ) {
        // Check that the tip Y coord is inside the editor
        // this is to prevent some zombie tips appearing floating in no-man-land
        wxRect editorRect = focusEdior->GetScreenRect();
        if ( editorRect.GetTopLeft().y > pt.y ) {
            m_positionedToRight = true;
            return;
        }
    }
    
    SetSize(wxRect(pt, GetSize()));
    Show();

    if( focusEdior ) {
        focusEdior->SetActive();
    }
}

void CCBoxTipWindow::OnEraseBG(wxEraseEvent& e)
{
    wxUnusedVar(e);
}

void CCBoxTipWindow::OnPaint(wxPaintEvent& e)
{
    wxBufferedPaintDC dc(this);

    wxColour penColour   = wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT);
    wxColour brushColour = wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK);

    dc.SetBrush( brushColour   );
    dc.SetPen  ( *wxBLACK_PEN );

    wxRect rr = GetClientRect();
    dc.DrawRectangle(rr);
    dc.SetPen  ( penColour );

    // Draw left-right arrows
    if ( m_numOfTips > 1 && m_leftbmp.IsOk() && m_rightbmp.IsOk() ) {
        
        // If the tip is positioned on the right side of the completion box
        // place the <- -> buttons on the LEFT side
        int leftButtonOffset = 5;
        if ( !m_positionedToRight ) {
            // Place the buttons on the RIGHT side
            leftButtonOffset = rr.GetWidth() - m_leftbmp.GetWidth() - m_rightbmp.GetWidth() - 10; // 10 pixels padding
        }
        
        m_leftTipRect  = wxRect(leftButtonOffset, 5, m_leftbmp.GetWidth(), m_leftbmp.GetHeight());
        wxPoint rightBmpPt = m_leftTipRect.GetTopRight();
        rightBmpPt.x += 5;
        m_rightTipRect = wxRect(rightBmpPt, wxSize(m_rightbmp.GetWidth(), m_rightbmp.GetHeight()));

        dc.DrawBitmap(m_leftbmp,  m_leftTipRect.GetTopLeft());
        dc.DrawBitmap(m_rightbmp, m_rightTipRect.GetTopLeft());
        
    } else {
        m_leftTipRect = wxRect();
        m_rightTipRect = wxRect();
    }

    dc.SetFont(m_commentFont);
    dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));

    ::setMarkupLexerInput(m_tip);
    wxString curtext;

    size_t i=0;
    wxPoint pt(5, 0);
    while ( true ) {
        int type = ::MarkupLex();

        if ( type == 0 )  // EOF
            break;

        switch (type) {
        case BOLD_START: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt);
            wxFont f = dc.GetFont();
            f.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetFont(f);
            break;
        }
        case BOLD_END: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt);

            wxFont f = dc.GetFont();
            f.SetWeight(wxFONTWEIGHT_NORMAL);
            dc.SetFont(f);
            break;
        }
        case CODE_START: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt);

            dc.SetFont(m_codeFont);
            break;
        }
        case CODE_END: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt);

            dc.SetFont(m_commentFont);
            break;
        }

        case NEW_LINE: {
            // New line, print the content
            DoPrintText(dc, curtext, pt);
            ++i;

            // reset the drawing point to the start of the next line
            pt = wxPoint(5, i*m_lineHeight);
            break;
        }
        case HORIZONTAL_LINE: {
            // Draw the buffer
            curtext.Clear();
            pt = wxPoint(5, i*m_lineHeight);
            dc.DrawLine(wxPoint(0, pt.y), wxPoint(rr.GetWidth(), pt.y));

            pt = wxPoint(5, i*m_lineHeight);
            break;
        }
        case COLOR_START: {
            DoPrintText(dc, curtext, pt);
            wxString colorname = ::MarkupText();
            colorname = colorname.AfterFirst(wxT('"'));
            colorname = colorname.BeforeLast(wxT('"'));
            dc.SetTextForeground(wxColour(colorname));
            break;
        }
        case COLOR_END: {
            DoPrintText(dc, curtext, pt);
            // restore default colour
            dc.SetTextForeground(penColour);
            break;
        }
        default:
            curtext << ::MarkupText();
            break;
        }
    }

    if ( curtext.IsEmpty() == false ) {
        DoPrintText(dc, curtext, pt);
    }

    ::MarkupClean();
}

void CCBoxTipWindow::DoPrintText(wxDC& dc, wxString& text, wxPoint& pt)
{
    if ( text.IsEmpty() == false ) {
        wxSize sz = dc.GetTextExtent(text);
        dc.DrawText(text, pt);
        pt.x += sz.x;
        text.Clear();
    }
}

void CCBoxTipWindow::OnMouseLeft(wxMouseEvent& e)
{
    if ( m_leftTipRect.Contains( e.GetPosition() ) )  {
        wxCommandEvent evt(wxEVT_TIP_BTN_CLICKED_UP);
        EventNotifier::Get()->AddPendingEvent(evt);

    } else if ( m_rightTipRect.Contains( e.GetPosition() ) ) {
        wxCommandEvent evt(wxEVT_TIP_BTN_CLICKED_DOWN);
        EventNotifier::Get()->AddPendingEvent(evt);

    } else {
        e.Skip();
    }
}

wxString CCBoxTipWindow::DoStripMarkups()
{
    ::setMarkupLexerInput(m_tip);
    wxString text;
    while ( true ) {
        int type = ::MarkupLex();

        if ( type == 0 )  // EOF
            break;
        switch (type) {
        case BOLD_START:
        case BOLD_END:
        case CODE_START:
        case CODE_END:
        case COLOR_START:
        case COLOR_END:
            break;
        case HORIZONTAL_LINE:
            text << "<hr>";
        case NEW_LINE:
            text << "\n";
            break;
        default:
            text << ::MarkupText();
            break;
        }
    }

    ::MarkupClean();
    return text;
}

void CCBoxTipWindow::PositionAt(const wxPoint& pt, LEditor* focusEdior)
{
    SetSize(wxRect(pt, GetSize()));
    Show();

    if( focusEdior ) {
        focusEdior->SetActive();
    }
}

