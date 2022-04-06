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

#include "cc_box_tip_window.h"

#include "ColoursAndFontsManager.h"
#include "Markup.h"
#include "bitmap_loader.h"
#include "clMarkdownRenderer.hpp"
#include "clSystemSettings.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "wx/arrstr.h"
#include "wx/dcclient.h"
#include "wx/panel.h"
#include "wx/regex.h"
#include "wx/sizer.h"

#include <algorithm>
#include <memory>
#include <wx/bitmap.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/display.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/stc/stc.h>
#include <wx/tokenzr.h>

const wxEventType wxEVT_TIP_BTN_CLICKED_UP = wxNewEventType();
const wxEventType wxEVT_TIP_BTN_CLICKED_DOWN = wxNewEventType();

namespace
{
/// mostly needed under linux where the calculate size does not match the actual size required to draw the tooltip
/// this is probably due to different wxDC types used
double m_ratio = 0.0;

void InflateSize(wxSize& size, double factor)
{
    if(factor > 1.0) {
        double updated_w = factor * (double)size.GetWidth();
        double updated_h = factor * (double)size.GetHeight();
        size.SetWidth(updated_w);
        size.SetHeight(updated_h);
    }
}

void CCBoxTipWindow_ShrinkTip(wxString& str, bool strip_html_tags)
{
    wxString restr = R"str(<.*?>)str";
    wxRegEx re(restr, wxRE_ADVANCED);

    auto lines = wxStringTokenize(str, "\n", wxTOKEN_RET_EMPTY);
    str.clear();

    wxArrayString lines_trimmed;
    lines_trimmed.reserve(lines.size());

    for(auto& line : lines) {
        if(strip_html_tags && re.IsValid()) {
            re.ReplaceAll(&line, wxEmptyString);
        }

        line.Replace("\t", " ");
        line.Replace("/**", wxEmptyString);
        line.Replace("/*", wxEmptyString);
        line.Replace("/*!", wxEmptyString);
        line.Replace("*/", wxEmptyString);

        bool cont = true;
        size_t len = 0;
        for(size_t i = 0; i < line.length() && cont; ++i) {
            wxChar ch = line[i];
            switch(ch) {
            case ' ':
                // skip leading whitespace
                break;
            case '*':
                // include the start and break
                len = i + 1;
                cont = false;
                break;
            default:
                cont = false;
                break;
            }
        }

        if(len) {
            line.erase(0, len);
        }

        // check if this line is empty
        wxString tmp_line = line;
        tmp_line.Trim();

        bool is_empty_line = tmp_line.empty();

        // remove excessive empty lines
        if(is_empty_line && (lines_trimmed.empty() || lines_trimmed.Last().empty()))
            continue;

        if(is_empty_line) {
            lines_trimmed.Add(tmp_line);
        } else {
            lines_trimmed.Add(line);
        }
    }
    str = wxJoin(lines_trimmed, '\n');
}

/**
 * @brief return the display for a given window
 * this function never fails
 */
std::unique_ptr<wxDisplay> GetDisplay(const wxWindow* win)
{
#if wxCHECK_VERSION(3, 1, 2)
    wxDisplay* d = new wxDisplay(win);
#else
    wxDisplay* d = nullptr;
    int index = wxDisplay::GetFromWindow(win);
    if(index == wxNOT_FOUND) {
        d = new wxDisplay();
    } else {
        d = new wxDisplay(index);
    }
#endif
    return std::unique_ptr<wxDisplay>(d);
}
} // namespace

CCBoxTipWindow::CCBoxTipWindow(wxWindow* parent, const wxString& tip, bool strip_html_tags)
    : wxPopupWindow(parent)
    , m_tip(tip)
    , m_stripHtmlTags(strip_html_tags)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    CCBoxTipWindow_ShrinkTip(m_tip, m_stripHtmlTags);
    DoInitialize(1);

    Bind(wxEVT_PAINT, &CCBoxTipWindow::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &CCBoxTipWindow::OnEraseBG, this);
}

CCBoxTipWindow::~CCBoxTipWindow() {}

void CCBoxTipWindow::DoInitialize(size_t numOfTips)
{
    m_numOfTips = numOfTips;
    wxClientDC dc(this);

    clMarkdownRenderer renderer;
    wxRect text_rect = renderer.GetSize(this, dc, m_tip);
    if(m_ratio > 0.0) {
        wxSize sz = text_rect.GetSize();
        InflateSize(sz, m_ratio);
        text_rect.SetSize(sz);
    }
    text_rect.Inflate(5);

    wxSize shrinked_size = text_rect.GetSize();
    ShrinkToScreen(shrinked_size);
    text_rect.SetSize(shrinked_size);

    SetSizeHints(text_rect.GetSize());
    SetSize(text_rect.GetSize());
    Layout();
}

void CCBoxTipWindow::ShrinkToScreen(wxSize& size) const
{
    // make sure that the tip window, is not bigger than the screen
    auto display = GetDisplay(this);
    // shrink up to the client area to preserve an extra space to un-hover it
    wxRect display_rect = display->GetClientArea();

    if(size.GetHeight() > display_rect.GetHeight()) {
        size.SetHeight(display_rect.GetHeight());
    }
    if(size.GetWidth() >= display_rect.GetWidth()) {
        size.SetWidth(display_rect.GetWidth());
    }
}

void CCBoxTipWindow::PositionRelativeTo(wxWindow* win, wxPoint caretPos, IEditor* focusEdior)
{
    // When shown, set the focus back to the editor
    wxPoint pt = win->GetScreenPosition();
    wxPoint windowPos = pt;
    wxSize ccBoxSize = win->GetSize();
    wxSize tipSize = GetSize();
    pt.x += ccBoxSize.x;
    int lineHeight = 20;
    wxStyledTextCtrl* stc = nullptr;
    if(focusEdior) {
        stc = focusEdior->GetCtrl();
    } else {
        IEditor* editor = clGetManager()->GetActiveEditor();
        if(editor) {
            stc = editor->GetCtrl();
        }
    }

    if(stc) {
        lineHeight = stc->TextHeight(stc->GetCurrentLine());
    }

    bool ccBoxIsAboveCaretLine = (windowPos.y < caretPos.y);
    // Check for overflow
    bool vPositioned = false;

    auto display = GetDisplay(this);
    wxRect displaySize = display->GetGeometry();

    if((pt.x + tipSize.x) > (displaySize.GetX() + displaySize.GetWidth())) {
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
                pt.y -= lineHeight; // The CC box is placed under the caret line, but the tip will be placed
                                    // on top of the CC box
            }

            if(pt.y < 0) {
                // try placing under the completion box
                pt = windowPos;
                pt.y += ccBoxSize.y + 1;
                if(ccBoxIsAboveCaretLine) {
                    pt.y += lineHeight; // dont hide the caret line
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
        if(editorRect.GetTopLeft().y > pt.y) {
            return;
        }
    }

    SetSize(wxRect(pt, GetSize()));
    Show();

    if(focusEdior) {
        focusEdior->SetActive();
    }
}

void CCBoxTipWindow::OnEraseBG(wxEraseEvent& e) { wxUnusedVar(e); }

void CCBoxTipWindow::OnPaint(wxPaintEvent& e)
{
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);
    DoDrawTip(dc);
}

void CCBoxTipWindow::PositionAt(const wxPoint& pt, IEditor* focusEdior)
{
    SetSize(wxRect(pt, GetSize()));
    Show();

    if(focusEdior) {
        focusEdior->SetActive();
    }
}

void CCBoxTipWindow::PositionLeftTo(wxWindow* win, IEditor* focusEditor)
{
    // Move the tip to the left
    wxPoint pt = win->GetScreenPosition();
    pt.x -= GetSize().x;

    SetSize(wxRect(pt, GetSize()));
    Show();

    if(focusEditor) {
        focusEditor->SetActive();
    }
}

void CCBoxTipWindow::DoDrawTip(wxDC& dc)
{
    clMarkdownRenderer renderer;
    wxSize size = renderer.Render(this, dc, m_tip, GetClientRect());
    wxSize client_rect = GetClientRect().GetSize();

    ShrinkToScreen(size);

    if(m_ratio == 0.0 && size.GetWidth() > client_rect.GetWidth()) {
        m_ratio = (double)size.GetWidth() / (double)client_rect.GetWidth();
        m_ratio += 0.01; // give it some extra space

        InflateSize(client_rect, m_ratio);
        SetSizeHints(client_rect);
        SetSize(client_rect);
    }
}
