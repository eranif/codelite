//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : theme_handler_helper.cpp
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

#include "ColoursAndFontsManager.h"
#include "Notebook.h"
#include "clTabRendererClassic.h"
#include "clTabRendererCurved.h"
#include "clTabRendererSquare.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "plugin.h"
#include "theme_handler_helper.h"
#include <algorithm>
#include <wx/aui/auibar.h>
#include <wx/bitmap.h>
#include <wx/button.h>
#include <wx/dataview.h>
#include <wx/html/htmlwin.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>
#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include "clSystemSettings.h"
#if USE_AUI_NOTEBOOK
#include "clAuiMainNotebookTabArt.h"
#include <wx/aui/tabart.h>
#endif

#define IS_TYPEOF(Type, Win) (dynamic_cast<Type*>(Win))

ThemeHandlerHelper::ThemeHandlerHelper(wxWindow* win)
    : m_window(win)
{
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ThemeHandlerHelper::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ThemeHandlerHelper::OnColoursUpdated, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &ThemeHandlerHelper::OnPreferencesUpdated, this);
}

ThemeHandlerHelper::~ThemeHandlerHelper()
{
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ThemeHandlerHelper::OnColoursUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &ThemeHandlerHelper::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &ThemeHandlerHelper::OnPreferencesUpdated, this);
}

void ThemeHandlerHelper::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    UpdateColours(m_window);
}

void ThemeHandlerHelper::UpdateColours(wxWindow* topWindow)
{
    // Collect all toolbars
    std::queue<wxWindow*> q;
    std::vector<wxAuiToolBar*> toolbars;
    std::vector<wxWindow*> candidateWindows;
    q.push(topWindow);

    wxColour bgColour = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    wxColour fgColour = clSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);

    LexerConf::Ptr_t textLexer = EditorConfigST::Get()->GetLexer("text");
    while(!q.empty()) {
        wxWindow* w = q.front();
        q.pop();
        if(dynamic_cast<wxAuiToolBar*>(w)) {
            toolbars.push_back(dynamic_cast<wxAuiToolBar*>(w));
        } else {
            if(IS_TYPEOF(wxListBox, w) || IS_TYPEOF(wxDataViewCtrl, w) || IS_TYPEOF(wxListCtrl, w)) {
                w->SetBackgroundColour(bgColour);
                w->SetForegroundColour(fgColour);
                w->Refresh();
            } else if(IS_TYPEOF(wxStyledTextCtrl, w)) {
                // wxSTC requires different method
                wxStyledTextCtrl* stc = dynamic_cast<wxStyledTextCtrl*>(w);
                if(stc->GetLexer() == wxSTC_LEX_NULL) {
                    if(!textLexer) {
                        // Only modify text lexers
                        for(int i = 0; i < wxSTC_STYLE_MAX; i++) {
                            stc->StyleSetBackground(i, bgColour);
                            stc->StyleSetForeground(i, fgColour);
                        }

                    } else {
                        textLexer->Apply(stc);
                    }
                }
                w->Refresh();
            }
            wxWindowList::compatibility_iterator iter = w->GetChildren().GetFirst();
            while(iter) {
                q.push(iter->GetData());
                iter = iter->GetNext();
            }
        }
    }

    //    std::for_each(toolbars.begin(), toolbars.end(), [&](wxAuiToolBar* tb) {
    //        // Update the art if needed
    //        CLMainAuiTBArt* art = dynamic_cast<CLMainAuiTBArt*>(tb->GetArtProvider());
    //        if(!art) { tb->SetArtProvider(new CLMainAuiTBArt()); }
    //
    //#ifndef __WXOSX__
    //        for(size_t i = 0; i < tb->GetToolCount(); ++i) {
    //            wxAuiToolBarItem* tbItem = tb->FindToolByIndex(i);
    //            if(tbItem->GetBitmap().IsOk() &&
    //               (tbItem->GetKind() == wxITEM_NORMAL || tbItem->GetKind() == wxITEM_CHECK ||
    //                tbItem->GetKind() == wxITEM_DROPDOWN || tbItem->GetKind() == wxITEM_RADIO)) {
    //                tbItem->SetDisabledBitmap(DrawingUtils::CreateDisabledBitmap(tbItem->GetBitmap()));
    //            }
    //        }
    //#endif
    //        tb->Refresh();
    //    });

    DoUpdateNotebookStyle(m_window);
}

#if USE_AUI_NOTEBOOK
class MySimpleTabArt : public wxAuiSimpleTabArt
{
    wxColour m_activeTabBgColour;

public:
    MySimpleTabArt() { m_activeTabBgColour = *wxWHITE; }
    virtual ~MySimpleTabArt() {}
    wxAuiTabArt* Clone() { return new MySimpleTabArt(*this); }

    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState,
                 wxRect* outTabRect, wxRect* outButtonRect, int* xExtent)
    {
        if(pane.active) {
            if(DrawingUtils::IsDark(m_activeTabBgColour)) { dc.SetTextForeground(*wxWHITE); }
        } else {
            // set the default text colour
            dc.SetTextForeground(DrawingUtils::GetPanelTextColour());
        }
        wxAuiSimpleTabArt::DrawTab(dc, wnd, pane, inRect, closeButtonState, outTabRect, outButtonRect, xExtent);
    }

    void SetActiveColour(const wxColour& colour)
    {
        m_activeTabBgColour = colour;
        wxAuiSimpleTabArt::SetActiveColour(colour);
    }
};
class MyDefaultTabArt : public wxAuiGenericTabArt
{
    wxColour m_activeTabBgColour;

public:
    MyDefaultTabArt() { m_activeTabBgColour = *wxWHITE; }
    virtual ~MyDefaultTabArt() {}
    wxAuiTabArt* Clone() { return new MyDefaultTabArt(*this); }

    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState,
                 wxRect* outTabRect, wxRect* outButtonRect, int* xExtent)
    {
        if(pane.active) {
            if(DrawingUtils::IsDark(m_activeTabBgColour)) { dc.SetTextForeground(*wxWHITE); }
        } else {
            // set the default text colour
            dc.SetTextForeground(DrawingUtils::GetPanelTextColour());
        }
        wxAuiGenericTabArt::DrawTab(dc, wnd, pane, inRect, closeButtonState, outTabRect, outButtonRect, xExtent);
    }

    void SetActiveColour(const wxColour& colour)
    {
        m_activeTabBgColour = colour;
        wxAuiGenericTabArt::SetActiveColour(colour);
    }
};
#endif
void ThemeHandlerHelper::DoUpdateNotebookStyle(wxWindow* win)
{
    if(!win) { return; }
    if(dynamic_cast<Notebook*>(win)) {
        Notebook* book = dynamic_cast<Notebook*>(win);
        book->SetArt(clTabRenderer::CreateRenderer(book, book->GetStyle()));
        LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
        wxColour activeTabBgColuor;
        if(lexer) { activeTabBgColuor = lexer->GetProperty(0).GetBgColour(); }

        // Enable tab switching using the mouse scrollbar
        book->EnableStyle(kNotebook_MouseScrollSwitchTabs,
                          EditorConfigST::Get()->GetOptions()->IsMouseScrollSwitchTabs());
    }
    
    wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
    while(pclNode) {
        wxWindow* pclChild = pclNode->GetData();
        this->DoUpdateNotebookStyle(pclChild);
        pclNode = pclNode->GetNext();
    }
}

void ThemeHandlerHelper::OnPreferencesUpdated(wxCommandEvent& e)
{
    e.Skip();
    DoUpdateNotebookStyle(m_window);
}

void ThemeHandlerHelper::OnColoursUpdated(clCommandEvent& e) { e.Skip(); }

void ThemeHandlerHelper::UpdateNotebookColours(wxWindow* topWindow) { DoUpdateNotebookStyle(topWindow); }
