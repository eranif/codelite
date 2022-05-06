//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cl_editor_tip_window.cpp
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
//    it under the terms OF the GNU General Public License as published by
//    the Free Software Foundation; either version 2 OF the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "cl_editor_tip_window.h"

#include "ColoursAndFontsManager.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "plugin.h"

#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/stc/stc.h>

BEGIN_EVENT_TABLE(clEditorTipWindow, wxPanel)
EVT_PAINT(clEditorTipWindow::OnPaint)
EVT_ERASE_BACKGROUND(clEditorTipWindow::OnEraseBg)
END_EVENT_TABLE()

#define TIP_SPACER 10

clEditorTipWindow::clEditorTipWindow(wxWindow* parent)
    : wxPanel(parent)
    , m_highlighIndex(0)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_font = ColoursAndFontsManager::Get().GetFixedFont(true);
    Hide();
    EventNotifier::Get()->Connect(wxEVT_CMD_COLOURS_FONTS_UPDATED,
                                  clCommandEventHandler(clEditorTipWindow::OnEditoConfigChanged), NULL, this);
}

clEditorTipWindow::~clEditorTipWindow()
{
    EventNotifier::Get()->Disconnect(wxEVT_CMD_COLOURS_FONTS_UPDATED,
                                     clCommandEventHandler(clEditorTipWindow::OnEditoConfigChanged), NULL, this);
}

void clEditorTipWindow::OnEraseBg(wxEraseEvent& e) { wxUnusedVar(e); }

void clEditorTipWindow::OnPaint(wxPaintEvent& e)
{
    wxUnusedVar(e);
    wxAutoBufferedPaintDC dc(this);
    PrepareDC(dc);

    if(m_args.IsEmpty())
        return;

    // Define the colours used by this tooltip window
    clColours colours = DrawingUtils::GetColours();
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(editor) {
        wxColour bgColour = editor->GetCtrl()->StyleGetBackground(0);
        if(DrawingUtils::IsDark(bgColour)) {
            colours.InitFromColour(bgColour);
        } else {
            colours.InitFromColour(clSystemSettings::GetDefaultPanelColour());
        }
    }

    wxColour bgColour, penColour, textColour, highlightBgColour, highlightFgColour;
    bgColour = colours.GetBgColour();
    penColour = colours.GetBorderColour();
    textColour = colours.GetItemTextColour();
    highlightBgColour = colours.GetSelItemBgColour();
    highlightFgColour = colours.GetSelItemTextColour();
    wxRect rr = GetClientRect();

    bool is_dark = DrawingUtils::IsDark(bgColour);

#ifdef __WXMAC__
    rr.Inflate(1);
#endif

    // draw the background using the parent background colour
    dc.SetBrush(bgColour);
    wxUnusedVar(is_dark);

    wxColour borderColour = clSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
    dc.SetPen(borderColour);
    dc.DrawRectangle(rr);
    dc.SetFont(m_font);

    // Highlight the text
    dc.SetTextForeground(textColour);
    wxSize helperTextSize = dc.GetTextExtent("Tp");

    wxCoord x = TIP_SPACER;
    wxCoord y = 0;

    if(!m_header.IsEmpty()) {
        wxFont guiFont = m_font;
        dc.SetFont(guiFont);
        wxSize headerSize = dc.GetTextExtent(m_header);
        wxPoint headerPt;
        headerPt.x = rr.GetWidth() - headerSize.x - TIP_SPACER;
        headerPt.y = 0;
        dc.SetTextForeground(textColour);
        dc.DrawText(m_header, headerPt);
        y += helperTextSize.y;
        dc.DrawLine(0, y, rr.GetWidth(), y);
    }

    dc.SetFont(m_font);
    for(size_t i = 0; i < m_args.size(); ++i) {
        wxString line = m_args.Item(i);
        if((int)i == m_highlighIndex) {
            // wxFont f = m_font;
            // f.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetBrush(highlightBgColour);
            dc.SetPen(highlightBgColour);
            dc.SetTextForeground(highlightFgColour);
            wxRect selectionRect(0, y, rr.GetWidth(), helperTextSize.y);
            selectionRect.Deflate(1);
            dc.DrawRectangle(selectionRect);
            dc.DrawText(line, x, y);

        } else {
            dc.SetTextForeground(textColour);
            dc.DrawText(line, x, y);
        }
        y += helperTextSize.y;
    }

    if(!m_footer.IsEmpty()) {
        dc.SetPen(penColour);
        dc.DrawLine(0, y, rr.GetWidth(), y);

        // Draw the extra line ("1 OF N")
        m_footer.Clear();
        m_footer << (GetTip()->GetCurr() + 1) << " OF " << GetTip()->Count();

        wxFont guiFont = m_font;
        dc.SetFont(guiFont);
        wxSize extraLineSize = dc.GetTextExtent(m_footer);

        wxPoint extraLinePt;
        extraLinePt.x = rr.GetWidth() - extraLineSize.x - TIP_SPACER;
        extraLinePt.y = y + 2;
        dc.SetTextForeground(textColour);
        dc.DrawText(m_footer, extraLinePt);
    }
}

void clEditorTipWindow::AddCallTip(clCallTipPtr tip)
{
    if(tip && tip->Count()) {
        TipInfo ti;
        ti.tip = tip;
        ti.highlightIndex = 0;
        m_highlighIndex = 0;
        if(!m_selectedSignature.IsEmpty()) {
            tip->SelectSiganture(m_selectedSignature);
            m_selectedSignature.Clear();
        }
        m_tips.push_back(ti);
    }
}

clCallTipPtr clEditorTipWindow::GetTip()
{
    if(m_tips.empty())
        return NULL;

    return m_tips.at(m_tips.size() - 1).tip;
}

bool clEditorTipWindow::IsEmpty() { return m_tips.empty(); }

void clEditorTipWindow::Remove()
{
    if(!m_tips.empty()) {
        m_tips.pop_back();

        if(!m_tips.empty()) {
            m_highlighIndex = m_tips.at(m_tips.size() - 1).highlightIndex;
        }
    }

    if(m_tips.empty())
        Deactivate();
}

void clEditorTipWindow::Clear()
{
    m_tips.clear();
    m_tipText.Clear();

    m_highlighIndex = 0;
    m_point = wxPoint();
    m_lineHeight = 0;
}

bool clEditorTipWindow::IsActive() { return IsShown(); }

void clEditorTipWindow::Highlight(int argIdxToHilight)
{
    clCallTipPtr tip = GetTip();
    if(tip) {
        if(argIdxToHilight == wxNOT_FOUND) {
            Remove();
        } else {
            m_tipText = tip->Current();
            DoMakeMultipleLineTip();
            if((int)m_args.size() <= argIdxToHilight) {
                // The current tip does not match the requested arg to highlight
                // try to find a better tip
                if(!tip->SelectTipToMatchArgCount(argIdxToHilight)) {
                    Deactivate();
                    return;
                }

                // Update the tip text
                tip = GetTip();
                if(!tip) {
                    Deactivate();
                    return;
                }
                m_tipText = tip->Current();
                DoMakeMultipleLineTip();
            }
            m_highlighIndex = argIdxToHilight;
            DoLayoutTip();
        }
    } else {
        Deactivate();
    }
}

void clEditorTipWindow::SelectNext(int argIdxToHilight)
{
    clCallTipPtr tip = GetTip();
    if(tip) {
        m_tipText = tip->Next();
        DoMakeMultipleLineTip();
        m_highlighIndex = argIdxToHilight;
        DoLayoutTip();
    }
}

void clEditorTipWindow::SelectPrev(int argIdxToHilight)
{
    clCallTipPtr tip = GetTip();
    if(tip) {
        m_tipText = tip->Prev();
        DoMakeMultipleLineTip();
        m_highlighIndex = argIdxToHilight;
        DoLayoutTip();
    }
}

wxString clEditorTipWindow::GetText()
{
    clCallTipPtr tip = GetTip();
    if(tip) {
        return tip->All();
    }
    return wxT("");
}

void clEditorTipWindow::Activate(wxPoint pt, int lineHeight, wxColour parentBgColour)
{
    if(m_tips.empty())
        return;

    m_point = pt;
    m_lineHeight = lineHeight;
    m_parentBgColour = parentBgColour;

    // update the font to the current editor
    m_font = ColoursAndFontsManager::Get().GetFixedFont(true);

    DoAdjustPosition();
    if(!IsActive()) {
        Show();
        Refresh();
    }
}

void clEditorTipWindow::Deactivate()
{
    Clear();
    if(IsShown())
        Hide();
}

wxSize clEditorTipWindow::DoGetTipSize()
{
    wxClientDC dc(this);
    PrepareDC(dc);

    wxFont f = m_font;
    f.SetWeight(wxFONTWEIGHT_BOLD);

    dc.SetFont(f);
    wxSize helperTextSize = dc.GetTextExtent("Tp");

    int lineHeight = helperTextSize.y;
    int minLineWidth = wxNOT_FOUND;
    if(!m_footer.IsEmpty()) {
        // Multiple signatures
        minLineWidth = dc.GetTextExtent(m_footer).x;
    }

    if(!m_header.IsEmpty()) {
        wxSize headerSize = dc.GetTextExtent(m_header);
        minLineWidth = headerSize.x > minLineWidth ? headerSize.x : minLineWidth;
    }

    minLineWidth += (2 * TIP_SPACER);
    wxString tipContent = wxJoin(m_args, '\n');
    tipContent.Trim().Trim(false);

    wxSize sz;
    wxSize sz2;
    sz = dc.GetMultiLineTextExtent(tipContent);

    // add spacers
    sz.y = (m_args.size() * lineHeight);
    sz.x += (2 * TIP_SPACER);

    if(sz.x < minLineWidth) {
        sz.x = minLineWidth;
    }

    if(!m_footer.IsEmpty()) {
        sz.y += lineHeight;
    }
    if(!m_header.IsEmpty()) {
        sz.y += lineHeight;
    }
    return sz;
}

int clEditorTipWindow::DoGetTextLen(wxDC& dc, const wxString& txt)
{
    int xx, yy;
    dc.GetTextExtent(txt, &xx, &yy, NULL, NULL, &m_font);
    return xx;
}

void clEditorTipWindow::DoAdjustPosition()
{
    wxPoint pt = m_point;
    wxSize sz = DoGetTipSize();
    wxRect parentSize = GetParent()->GetClientRect();

    if(pt.y + m_lineHeight + sz.y > parentSize.height) {
        pt.y -= sz.y;

    } else {
        pt.y += m_lineHeight;
    }

    if(pt.x + sz.x > parentSize.width) {
        // our tip can not fit into the screen, shift it left
        pt.x -= ((pt.x + sz.x) - parentSize.width);

        if(pt.x < 0)
            pt.x = 0;
    }
    Move(pt);
}

void clEditorTipWindow::DoLayoutTip()
{
    SetSize(DoGetTipSize());
    SetSizeHints(DoGetTipSize());
    DoAdjustPosition();
    Layout();
    Refresh();
}

void clEditorTipWindow::SelectSignature(const wxString& signature)
{
    m_selectedSignature = signature;
    if(GetTip()) {
        GetTip()->SelectSiganture(m_selectedSignature);
        m_selectedSignature.clear();
    }
}

void clEditorTipWindow::OnEditoConfigChanged(clCommandEvent& e)
{
    e.Skip();
    // the lexers were modified by the user, reload the font
    m_font = ColoursAndFontsManager::Get().GetLexer("text")->GetFontForStyle(0, this);
    Refresh();
}

void clEditorTipWindow::DoMakeMultipleLineTip()
{
    // Find the open brace first
    m_args.Clear();
    m_footer.Clear();
    m_header.Clear();

    wxString sig = m_tipText.AfterFirst('(');
    wxString returnValue = m_tipText.BeforeFirst('(');
    returnValue.Trim().Trim(false);
    if(returnValue.EndsWith(":")) {
        returnValue.RemoveLast();
        returnValue.Trim().Trim(false);
        m_header << "RETURNS:  " << returnValue;
    }

    sig = sig.BeforeLast(')');
    sig.Trim().Trim(false);
    if(sig.IsEmpty()) {
        m_args.Add("()");
        if(GetTip() && (GetTip()->Count() > 1)) {
            m_footer << GetTip()->GetCurr() << " OF " << GetTip()->Count();
        }
        return;
    }

    int depth = 0;
    wxString currentArg;
    for(size_t i = 0; i < sig.length(); ++i) {
        wxChar ch = sig.at(i);
        if(ch == '<' || ch == '[' || ch == '{' || ch == '(') {
            ++depth;
            currentArg << ch;
        } else if(ch == '>' || ch == ']' || ch == '}' || ch == ')') {
            --depth;
            currentArg << ch;
            if(depth < 0) {
                // a parsing error
                m_tipText.Clear();
                m_args.Clear();
                return;
            }
        } else if(ch == ',' && (depth == 0)) {
            currentArg.Trim().Trim(false);
            m_args.Add(currentArg);
            currentArg.Clear();
        } else {
            currentArg << ch;
        }
    }

    if(!currentArg.IsEmpty()) {
        currentArg.Trim().Trim(false);
        m_args.Add(currentArg);
    }

    if(GetTip() && (GetTip()->Count() > 1)) {
        m_footer << GetTip()->GetCurr() << " OF " << GetTip()->Count();
    }
}
