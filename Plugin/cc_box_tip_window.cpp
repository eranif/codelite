//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cc_box_tip_window.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "Markup.h"
#include "bitmap_loader.h"
#include "cc_box_tip_window.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/stc/stc.h>
#include <wx/tokenzr.h>
#include <algorithm>

const wxEventType wxEVT_TIP_BTN_CLICKED_UP = wxNewEventType();
const wxEventType wxEVT_TIP_BTN_CLICKED_DOWN = wxNewEventType();

static void CCBoxTipWindow_ShrinkTip(wxString& str)
{
    str.Replace("\r", "");
    str.Replace("<p>", "");
    str.Replace("</p>", "");
    str.Replace("/**", "");
    str.Replace("/*!", "");
    str.Replace("/*", "");
    str.Replace("*/", "");
    str.Replace("**/", "");

    wxString tip;
    wxArrayString lines = ::wxStringTokenize(str, wxT("\n"), wxTOKEN_RET_EMPTY_ALL);
    for(size_t i = 0; i < lines.GetCount(); ++i) {
        wxString& curline = lines.Item(i);
        curline.Trim().Trim(false);
        if(curline.StartsWith("*")) curline.Remove(0, 1);
        tip << curline << "\n";
    }
    str.swap(tip);
    str.Trim().Trim(false);

    // strip double empty lines
    while(str.Replace("\n\n", "\n")) {}
}

CCBoxTipWindow::CCBoxTipWindow(wxWindow* parent, const wxString& tip)
    : wxPopupWindow(parent)
    , m_tip(tip)
    , m_useLightColours(false)
{
    CCBoxTipWindow_ShrinkTip(m_tip);
    DoInitialize(m_tip, 1, true);
}

CCBoxTipWindow::CCBoxTipWindow(wxWindow* parent, const wxString& tip, size_t numOfTips, bool simpleTip)
    : wxPopupWindow(parent)
    , m_tip(tip)
    , m_useLightColours(false)
{
    CCBoxTipWindow_ShrinkTip(m_tip);
    DoInitialize(m_tip, numOfTips, simpleTip);
}

CCBoxTipWindow::~CCBoxTipWindow() {}

void CCBoxTipWindow::DoInitialize(const wxString& tip, size_t numOfTips, bool simpleTip)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    IEditor* editor = ::clGetManager()->GetActiveEditor();
    if(editor) {
        wxColour bgColour = editor->GetCtrl()->StyleGetBackground(0);
        if(!DrawingUtils::IsDark(bgColour)) { m_useLightColours = true; }
    }

    m_tip = tip;
    m_numOfTips = numOfTips;

    // Invalidate the rectangles
    m_leftTipRect = wxRect();
    m_rightTipRect = wxRect();

    if(!simpleTip && m_numOfTips > 1) m_tip.Prepend(wxT("\n\n")); // Make room for the arrows

    Hide();

    wxBitmap bmp(1, 1);
    wxMemoryDC dc(bmp);

    wxSize size;

    m_codeFont = DrawingUtils::GetDefaultFixedFont();
    m_commentFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    wxString codePart, commentPart;
    wxString strippedTip = DoStripMarkups();

    size_t from = 0;
    int hr_count = 0;
    size_t hrPos = strippedTip.find("<hr>");
    while(hrPos != wxString::npos) {
        ++hr_count;
        from = hrPos + 4;
        hrPos = strippedTip.find("<hr>", from);
    }

    int where = strippedTip.Find("<hr>");
    if(where != wxNOT_FOUND) {
        codePart = strippedTip.Mid(0, where);
        commentPart = strippedTip.Mid(where + 5);

    } else {
        codePart = strippedTip;
    }

    int commentWidth = 0;
    int codeWidth = 0;

    // Use bold font for measurements
    // m_codeFont.SetWeight(wxFONTWEIGHT_BOLD);
    // m_commentFont.SetWeight(wxFONTWEIGHT_BOLD);

    if(!simpleTip) {
        dc.GetMultiLineTextExtent(codePart, &codeWidth, NULL, NULL, &m_codeFont);
        dc.GetMultiLineTextExtent(commentPart, &commentWidth, NULL, NULL, &m_commentFont);

    } else {
        dc.GetMultiLineTextExtent(strippedTip, &codeWidth, NULL, NULL, &m_commentFont);
    }

    m_codeFont.SetWeight(wxFONTWEIGHT_NORMAL);
    m_commentFont.SetWeight(wxFONTWEIGHT_NORMAL);

    // Set the width
    commentWidth > codeWidth ? size.x = commentWidth : size.x = codeWidth;

    // Shrink the tip
    m_tip.Replace("\r", "");
    while(m_tip.Replace("\n\n", "\n")) {}

    dc.GetTextExtent(wxT("Tp"), NULL, &m_lineHeight, NULL, NULL, &m_codeFont);
    int nLineCount = ::wxStringTokenize(m_tip, wxT("\r\n"), wxTOKEN_RET_EMPTY_ALL).GetCount();

    size.y = nLineCount * m_lineHeight;
    size.y += (hr_count * 10) + 10; // each <hr> uses 10 pixels height
    size.x += 40;

    size_t maxWidth(0);

    // Calc the width
    DoDrawTip(dc, maxWidth);
    size.x = maxWidth;
    SetSize(size);

    Connect(wxEVT_PAINT, wxPaintEventHandler(CCBoxTipWindow::OnPaint), NULL, this);
    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(CCBoxTipWindow::OnEraseBG), NULL, this);
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(CCBoxTipWindow::OnMouseLeft), NULL, this);
}

void CCBoxTipWindow::PositionRelativeTo(wxWindow* win, wxPoint caretPos, IEditor* focusEdior)
{
    // When shown, set the focus back to the editor
    wxPoint pt = win->GetScreenPosition();
    wxPoint windowPos = pt;
    wxSize ccBoxSize = win->GetSize();
    wxSize tipSize = GetSize();
    pt.x += ccBoxSize.x;

    bool ccBoxIsAboveCaretLine = (windowPos.y < caretPos.y);
    // Check for overflow
    bool vPositioned = false;
    wxSize displaySize = ::wxGetDisplaySize();
    if((pt.x + tipSize.x) > displaySize.x) {
        // Move the tip to the left
        pt = windowPos;
        pt.x -= tipSize.x;

        if(pt.x < 0) {
            // it cant be placed on the left side either
            // try placing it on top of the completion box
            pt = windowPos;
            vPositioned = true;
            pt.y -= tipSize.y;
            if(!ccBoxIsAboveCaretLine) {
                pt.y -= 20; // The CC box is placed under the caret line, but the tip will be placed
                            // on top of the CC box - use 20 pixels so we don't hide the caret line
            }

            if(pt.y < 0) {
                // try placing under the completion box
                pt = windowPos;
                pt.y += ccBoxSize.y + 1;
                if(ccBoxIsAboveCaretLine) {
                    pt.y += 20; // dont hide the caret line
                }
            }
        }
    }
    
    if(!vPositioned) {
        // The tip window is positioned to the left or right of the CC box
        // Check if the tip window is going outside of the display, if it is, move it up
        if((pt.y + tipSize.GetHeight()) > displaySize.GetHeight()) {
            pt.y = (displaySize.GetHeight() - tipSize.GetHeight());
            // Make sure that the top of the tip is always visible
            pt.y = std::max(0, pt.y);
        }
    }
    
    if(focusEdior) {
        // Check that the tip Y coord is inside the editor
        // this is to prevent some zombie tips appearing floating in no-man-land
        wxRect editorRect = focusEdior->GetCtrl()->GetScreenRect();
        if(editorRect.GetTopLeft().y > pt.y) { return; }
    }

    SetSize(wxRect(pt, GetSize()));
    Show();

    if(focusEdior) { focusEdior->SetActive(); }
}

void CCBoxTipWindow::OnEraseBG(wxEraseEvent& e) { wxUnusedVar(e); }

void CCBoxTipWindow::OnPaint(wxPaintEvent& e)
{
    m_links.clear();
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);
    size_t maxWidth(0);
    DoDrawTip(dc, maxWidth);
}

wxRect CCBoxTipWindow::DoPrintText(wxDC& dc, wxString& text, wxPoint& pt, size_t& maxWidth)
{
    if(text.IsEmpty() == false) {
        wxSize sz = dc.GetTextExtent(text);
        wxRect rect(pt, sz);
        dc.DrawText(text, pt);
        pt.x += sz.x;
        maxWidth = wxMax(maxWidth, pt.x);
        text.Clear();
        return rect;
    }
    return wxRect();
}

void CCBoxTipWindow::OnMouseLeft(wxMouseEvent& e)
{
    if(m_leftTipRect.Contains(e.GetPosition())) {
        wxCommandEvent evt(wxEVT_TIP_BTN_CLICKED_UP);
        EventNotifier::Get()->AddPendingEvent(evt);

    } else if(m_rightTipRect.Contains(e.GetPosition())) {
        wxCommandEvent evt(wxEVT_TIP_BTN_CLICKED_DOWN);
        EventNotifier::Get()->AddPendingEvent(evt);

    } else {
        for(size_t i = 0; i < m_links.size(); ++i) {
            if(m_links.at(i).m_rect.Contains(e.GetPosition())) {
                ::wxLaunchDefaultBrowser(m_links.at(i).m_url);
                break;
            }
        }
    }
}

wxString CCBoxTipWindow::DoStripMarkups()
{
    MarkupParser parser(m_tip);
    wxString text;
    while(parser.Next()) {
        int type = parser.GetType();
        switch(type) {
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
            text << parser.GetToken();
            break;
        }
    }
    return text;
}

void CCBoxTipWindow::PositionAt(const wxPoint& pt, IEditor* focusEdior)
{
    SetSize(wxRect(pt, GetSize()));
    Show();

    if(focusEdior) { focusEdior->SetActive(); }
}

void CCBoxTipWindow::PositionLeftTo(wxWindow* win, IEditor* focusEditor)
{
    // Move the tip to the left
    wxPoint pt = win->GetScreenPosition();
    pt.x -= GetSize().x;

    SetSize(wxRect(pt, GetSize()));
    Show();

    if(focusEditor) { focusEditor->SetActive(); }
}

void CCBoxTipWindow::DoDrawTip(wxDC& dc, size_t& max_width)
{
    clColourPalette colors = DrawingUtils::GetColourPalette();

    wxColour penColour = colors.penColour;
    wxColour brushColour = colors.bgColour;
    wxColour textColour = colors.textColour;
    wxColour linkColour("rgb(204, 153, 255)");

    if(m_useLightColours) {
        // Use different colours to match the editor theme
        linkColour = wxColour("rgb(51, 153, 255)");
    }

    dc.SetBrush(brushColour);
    dc.SetPen(wxPen(penColour, 1));

    wxRect rr = GetClientRect();
#ifdef __WXOSX__
    rr.Inflate(1, 1);
#endif
    dc.DrawRectangle(rr);

    // Draw left-right arrows
    m_leftTipRect = wxRect();
    m_rightTipRect = wxRect();

    dc.SetFont(m_commentFont);
    dc.SetTextForeground(textColour);

    wxString curtext;
    MarkupParser parser(m_tip);
    wxPoint pt(5, 5);
    while(parser.Next()) {
        int type = parser.GetType();
        switch(type) {
        case LINK_URL: {
            // Found URL
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt, max_width);

            curtext = parser.GetToken();
            wxString link_url = curtext;

            wxFont f = dc.GetFont();
            f.SetWeight(wxFONTWEIGHT_NORMAL);
            f.SetUnderlined(true);
            dc.SetFont(f);
            dc.SetTextForeground(linkColour);
            wxRect url_rect = DoPrintText(dc, curtext, pt, max_width);

            // keep info about this URL
            CCBoxTipWindow::Links link_info;
            link_info.m_rect = url_rect;
            link_info.m_url = link_url;
            m_links.push_back(link_info);

            // Restore font and colour
            f.SetUnderlined(false);
            dc.SetFont(f);
            dc.SetTextForeground(textColour);

            break;
        }
        case BOLD_START: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt, max_width);

            wxFont f = dc.GetFont();
            f.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetFont(f);
            dc.SetTextBackground(*wxWHITE);
            break;
        }
        case BOLD_END: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt, max_width);

            wxFont f = dc.GetFont();
            f.SetWeight(wxFONTWEIGHT_NORMAL);
            dc.SetFont(f);
            dc.SetTextBackground(textColour);
            break;
        }
        case ITALIC_START: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt, max_width);

            wxFont f = dc.GetFont();
            f.SetStyle(wxFONTSTYLE_ITALIC);
            dc.SetFont(f);
            break;
        }
        case ITALIC_END: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt, max_width);

            wxFont f = dc.GetFont();
            f.SetStyle(wxFONTSTYLE_NORMAL);
            dc.SetFont(f);
            break;
        }
        case CODE_START: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt, max_width);

            dc.SetFont(m_codeFont);
            break;
        }
        case CODE_END: {
            // Before we change the font, draw the buffer
            DoPrintText(dc, curtext, pt, max_width);

            dc.SetFont(m_commentFont);
            break;
        }

        case NEW_LINE: {
            // New line, print the content
            DoPrintText(dc, curtext, pt, max_width);

            pt.y += m_lineHeight;

            // reset the drawing point to the start of the next line
            pt.x = 5;
            break;
        }
        case HORIZONTAL_LINE: {
            // Draw the buffer
            curtext.Clear();
            pt.y += 5;
            dc.DrawLine(wxPoint(0, pt.y), wxPoint(rr.GetWidth(), pt.y));
            pt.y += 5;
            pt.x = 5;
            break;
        }
        case COLOR_START: {
            DoPrintText(dc, curtext, pt, max_width);

            wxString colorname = parser.GetToken();
            colorname = colorname.AfterFirst(wxT('"'));
            colorname = colorname.BeforeLast(wxT('"'));
            dc.SetTextForeground(wxColour(colorname));
            break;
        }
        case COLOR_END: {
            DoPrintText(dc, curtext, pt, max_width);

            // restore default colour
            dc.SetTextForeground(textColour);
            break;
        }
        case MARKUP_VOID:
            // do nothing
            break;
        default:
            curtext << parser.GetToken();
            break;
        }
    }

    if(curtext.IsEmpty() == false) { DoPrintText(dc, curtext, pt, max_width); }
    max_width += 5; // right side margin
}
