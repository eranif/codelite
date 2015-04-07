//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
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
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "cl_editor_tip_window.h"
#include <wx/msgdlg.h>
#include "drawingutils.h"
#include <wx/settings.h>
#include <wx/dcbuffer.h>
#include <wx/log.h>
#include <wx/dcgraph.h>
#include "globals.h"
#include "editor_config.h"
#include <wx/stc/stc.h>
#include "editor_config.h"
#include "event_notifier.h"
#include "plugin.h"
#include "ColoursAndFontsManager.h"

BEGIN_EVENT_TABLE(clEditorTipWindow, wxPanel)
EVT_PAINT(clEditorTipWindow::OnPaint)
EVT_ERASE_BACKGROUND(clEditorTipWindow::OnEraseBg)
END_EVENT_TABLE()

#define TIP_SPACER 4

clEditorTipWindow::clEditorTipWindow(wxWindow* parent)
    : wxPanel(parent)
    , m_highlighIndex(0)
{
    m_font = ColoursAndFontsManager::Get().GetLexer("c++")->GetFontForSyle(0); // default font
    Hide();
    EventNotifier::Get()->Connect(
        wxEVT_CMD_COLOURS_FONTS_UPDATED, clCommandEventHandler(clEditorTipWindow::OnEditoConfigChanged), NULL, this);
}

clEditorTipWindow::~clEditorTipWindow()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_CMD_COLOURS_FONTS_UPDATED, clCommandEventHandler(clEditorTipWindow::OnEditoConfigChanged), NULL, this);
}

void clEditorTipWindow::OnEraseBg(wxEraseEvent& e) { wxUnusedVar(e); }

void clEditorTipWindow::OnPaint(wxPaintEvent& e)
{
    wxUnusedVar(e);
    wxBufferedPaintDC dc(this);

    wxGCDC gdc;
    if(!DrawingUtils::GetGCDC(dc, gdc)) return;
    if(m_args.IsEmpty()) return;

    // Define the colours used by this tooltip window
    wxColour bgColour, penColour, textColour, higlighTextBg, higlighTextFg, higlighTextPen;
    bgColour = wxColour("#feffcd"); // basic colour
    penColour = bgColour.ChangeLightness(80);
    textColour = wxColour("#000023");
    higlighTextBg = wxColour("rgb(204, 153, 0)");
    higlighTextPen = higlighTextBg;
    higlighTextFg = *wxWHITE;

    wxRect rr = GetClientRect();

    // draw the background using the parent background colour
    gdc.SetBrush(bgColour);
    gdc.SetPen(penColour);
    gdc.DrawRectangle(rr);
    gdc.SetFont(m_font);

    // Highlight the text
    gdc.SetTextForeground(textColour);
    wxSize helperTextSize = gdc.GetTextExtent("Tp");

    wxCoord x = TIP_SPACER;
    wxCoord y = 0;
    gdc.SetTextForeground(textColour);
    for(size_t i = 0; i < m_args.size(); ++i) {
        wxString line = m_args.Item(i);
        if((int)i == m_highlighIndex) {
            wxFont f = m_font;
            f.SetWeight(wxFONTWEIGHT_BOLD);
            gdc.SetFont(f);
        } else {
            gdc.SetFont(m_font);
        }
        gdc.DrawText(line, x, y);
        y += helperTextSize.y;
    }

    if(!m_coutLine.IsEmpty()) {
        // Draw the extra line ("1 of N")
        m_coutLine.Clear();
        m_coutLine << (GetTip()->GetCurr() + 1) << " of " << GetTip()->Count();

        wxFont guiFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        gdc.SetFont(guiFont);
        wxSize extraLineSize = gdc.GetTextExtent(m_coutLine);

        wxPoint extraLinePt;
        extraLinePt.x = rr.GetWidth() - extraLineSize.x - TIP_SPACER;
        extraLinePt.y = rr.GetHeight() - extraLineSize.y;
        gdc.SetTextForeground(textColour);
        gdc.DrawText(m_coutLine, extraLinePt);
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
    if(m_tips.empty()) return NULL;

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

    if(m_tips.empty()) Deactivate();
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
        m_tipText = tip->Current();
        DoMakeMultipleLineTip();
        m_highlighIndex = argIdxToHilight;
        DoLayoutTip();
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
    if(m_tips.empty()) return;

    m_point = pt;
    m_lineHeight = lineHeight;
    m_parentBgColour = parentBgColour;

    DoAdjustPosition();
    if(!IsActive()) {
        Show();
    }
}

void clEditorTipWindow::Deactivate()
{
    Clear();
    if(IsShown()) Hide();
}

wxSize clEditorTipWindow::DoGetTipSize()
{
    wxDC* dc;

    wxGCDC gdc;
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);

    if(!DrawingUtils::GetGCDC(memDC, gdc)) {
        dc = (wxDC*)&memDC;
    } else {
        dc = (wxDC*)&gdc;
    }
    
    wxFont f = m_font;
    f.SetWeight(wxFONTWEIGHT_BOLD);
    
    dc->SetFont(f);
    wxSize helperTextSize = dc->GetTextExtent("Tp");

    int lineHeight = helperTextSize.y;
    int minLineWidth = wxNOT_FOUND;
    if(GetTip() && (GetTip()->Count() > 1)) {
        // Multiple signatures
        minLineWidth = dc->GetTextExtent("100 of 100").x;
    }

    wxString tipContent = wxJoin(m_args, '\n');
    tipContent.Trim().Trim(false);

    wxSize sz;
    wxSize sz2;
    sz = dc->GetMultiLineTextExtent(tipContent);

    // add spacers
    sz.y = (m_args.size() * lineHeight);
    sz.x += (2 * TIP_SPACER);

    if(sz.x < minLineWidth) {
        sz.x = minLineWidth;
    }

    if(minLineWidth != wxNOT_FOUND) {
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

        if(pt.x < 0) pt.x = 0;
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
    m_font = EditorConfigST::Get()->GetLexer("C++")->GetFontForSyle(wxSTC_STYLE_CALLTIP);
    Refresh();
}

void clEditorTipWindow::DoMakeMultipleLineTip()
{
    // Find the open brace first
    m_args.Clear();
    m_coutLine.Clear();

    wxString sig = m_tipText.AfterFirst('(');
    sig = sig.BeforeLast(')');
    sig.Trim().Trim(false);
    if(sig.IsEmpty()) {
        m_args.Add("()");
        if(GetTip() && (GetTip()->Count() > 1)) {
            m_coutLine << GetTip()->GetCurr() << " of " << GetTip()->Count();
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
        m_coutLine << GetTip()->GetCurr() << " of " << GetTip()->Count();
    }
}
