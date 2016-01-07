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

#include "theme_handler_helper.h"
#include "event_notifier.h"
#include "plugin.h"
#include "editor_config.h"
#include <wx/treectrl.h>
#include <wx/listbox.h>
#include <wx/dataview.h>
#include <wx/textctrl.h>
#include <wx/html/htmlwin.h>
#include "Notebook.h"
#include "clTabRendererSquare.h"
#include "clTabRendererCurved.h"

class wxDataViewCtrl;
class wxTextCtrl;
class wxListBox;

ThemeHandlerHelper::ThemeHandlerHelper(wxWindow* win)
    : m_window(win)
{
    EventNotifier::Get()->Connect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ThemeHandlerHelper::OnThemeChanged), NULL, this);
    EventNotifier::Get()->Connect(
        wxEVT_EDITOR_SETTINGS_CHANGED, wxCommandEventHandler(ThemeHandlerHelper::OnPreferencesUpdated), NULL, this);
}

ThemeHandlerHelper::~ThemeHandlerHelper()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ThemeHandlerHelper::OnThemeChanged), NULL, this);
    EventNotifier::Get()->Disconnect(
        wxEVT_EDITOR_SETTINGS_CHANGED, wxCommandEventHandler(ThemeHandlerHelper::OnPreferencesUpdated), NULL, this);
}

void ThemeHandlerHelper::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    wxColour bgColour = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
    wxColour fgColour = EditorConfigST::Get()->GetCurrentOutputviewFgColour();

    if(!bgColour.IsOk() || !fgColour.IsOk()) {
        return;
    }

    DoUpdateColours(m_window, bgColour, fgColour);
}

void ThemeHandlerHelper::DoUpdateColours(wxWindow* win, const wxColour& bg, const wxColour& fg)
{
    // wxTextCtrl needs some extra special handling
    if(dynamic_cast<wxTextCtrl*>(win)) {
        wxTextCtrl* txtCtrl = dynamic_cast<wxTextCtrl*>(win);
        wxTextAttr attr = txtCtrl->GetDefaultStyle();
        attr.SetBackgroundColour(bg);
        attr.SetTextColour(fg);
        txtCtrl->SetDefaultStyle(attr);
    }

    // Generic handling
    if(dynamic_cast<wxTreeCtrl*>(win) || dynamic_cast<wxListBox*>(win) || dynamic_cast<wxDataViewCtrl*>(win) ||
        dynamic_cast<wxTextCtrl*>(win) /*||
    dynamic_cast<wxHtmlWindow*>(win) */
        ) {
        win->SetBackgroundColour(bg);
        win->SetForegroundColour(fg);
        win->Refresh();
    }

    wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
    while(pclNode) {
        wxWindow* pclChild = pclNode->GetData();
        this->DoUpdateColours(pclChild, bg, fg);
        pclNode = pclNode->GetNext();
    }
}

void ThemeHandlerHelper::DoUpdateNotebookStyle(wxWindow* win)
{
    // wxTextCtrl needs some extra special handling
    if(dynamic_cast<Notebook*>(win)) {
        Notebook* book = dynamic_cast<Notebook*>(win);
        if(EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_TabStyleMinimal) {
            clTabRenderer::Ptr_t art(new clTabRendererSquare);
            book->SetArt(art);
        } else {
            clTabRenderer::Ptr_t art(new clTabRendererCurved);
            book->SetArt(art);
        }
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
