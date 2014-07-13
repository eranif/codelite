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

BEGIN_EVENT_TABLE(clEditorTipWindow, wxPanel)
EVT_PAINT(clEditorTipWindow::OnPaint)
EVT_ERASE_BACKGROUND(clEditorTipWindow::OnEraseBg)
END_EVENT_TABLE()

#define TIP_SPACER 4

clEditorTipWindow::clEditorTipWindow(wxWindow* parent)
    : wxPanel(parent)
    , m_highlighIndex(0)
{
    m_font = EditorConfigST::Get()->GetLexer("C++")->GetFontForSyle(wxSTC_STYLE_CALLTIP);
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
    if(!DrawingUtils::GetGCDC(dc, gdc))
        return;

    wxColour bgColour = DrawingUtils::GetThemeTipBgColour();
    wxColour penColour = DrawingUtils::GetThemeBorderColour();

    bool isDarkTheme = DrawingUtils::IsDark(bgColour);
    wxUnusedVar(isDarkTheme);

    wxRect rr = GetClientRect();

    // draw the background using the parent background colour
    gdc.SetBrush(bgColour);
    gdc.SetPen(penColour);
    gdc.DrawRectangle(rr);
    gdc.SetFont(m_font);

    // Highlight the text
    clCallTipPtr tip = GetTip();
    int secondLineY = (rr.GetHeight() / 2) + 1;
    int firstLineY = TIP_SPACER;

    // Draw the Tip text
    gdc.SetFont(m_font);
    gdc.SetTextForeground(EditorConfigST::Get()->GetCurrentOutputviewFgColour());
    gdc.DrawText(m_tipText, wxPoint(TIP_SPACER, firstLineY));

    if(tip) {
        wxString txt;
        txt << tip->GetCurr() + 1 << wxT(" of ") << tip->Count();
        int txtLen = DoGetTextLen(gdc, txt);

        int summaryLineXText(rr.GetWidth());
        summaryLineXText -= (txtLen + TIP_SPACER);

        // Draw the summary line
        gdc.SetFont(m_font);
        gdc.SetTextForeground(DrawingUtils::GetThemeTextColour());
        gdc.DrawText(txt, summaryLineXText, secondLineY + TIP_SPACER / 2);

        int start(-1), len(-1);
        tip->GetHighlightPos(m_highlighIndex, start, len);
        if(len != -1 && start != -1) {
            wxString txtBefore = m_tipText.Mid(0, start);
            wxString txtInclude = m_tipText.Mid(start, len);

            int x = DoGetTextLen(gdc, txtBefore);
            int w = DoGetTextLen(gdc, txtInclude);

            wxColour bodyColour, borderColour;
            wxColour base = *wxBLUE;
            bodyColour = base.ChangeLightness(180);
            borderColour = base.ChangeLightness(150);

            if(isDarkTheme) {
                gdc.SetBrush(*wxTRANSPARENT_BRUSH);
                gdc.SetPen(borderColour);
                gdc.DrawRoundedRectangle(
                    x + TIP_SPACER - 1, firstLineY - (TIP_SPACER / 2), w + 2, (rr.GetHeight() / 2), 3.5);

            } else {
                gdc.SetBrush(bodyColour);
                gdc.SetPen(borderColour);
                gdc.DrawRoundedRectangle(
                    x + TIP_SPACER - 1, firstLineY - (TIP_SPACER / 2), w + 2, (rr.GetHeight() / 2), 3.5);

                wxFont f = m_font;
                f.SetWeight(wxFONTWEIGHT_BOLD);
                gdc.SetFont(f);
                gdc.DrawText(txtInclude, wxPoint(x + TIP_SPACER, firstLineY));
            }
        }
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
        m_tipText = tip->Current();
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
        m_highlighIndex = argIdxToHilight;
        DoLayoutTip();
    }
}

void clEditorTipWindow::SelectPrev(int argIdxToHilight)
{
    clCallTipPtr tip = GetTip();
    if(tip) {
        m_tipText = tip->Prev();
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

    DoAdjustPosition();
    if(!IsActive()) {
        Show();
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
    wxDC* dc;

    wxGCDC gdc;
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);

    if(!DrawingUtils::GetGCDC(memDC, gdc)) {
        dc = (wxDC*)&memDC;
    } else {
        dc = (wxDC*)&gdc;
    }

    wxSize sz;
    wxSize sz2;
    dc->SetFont(m_font);
    sz = dc->GetTextExtent(m_tipText);
    sz2 = dc->GetTextExtent(wxT("100 of 100"));

    sz.y *= 2;
    sz.y += (2 * TIP_SPACER);
    sz.x += (2 * TIP_SPACER);

    if(sz.x < sz2.x) {
        sz.x = sz2.x;
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

    // by default place the tip below the caret

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
