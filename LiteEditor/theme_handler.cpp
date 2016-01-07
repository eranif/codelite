//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : theme_handler.cpp
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

#include "theme_handler.h"
#include "editor_config.h"
#include "frame.h"
#include "workspace_pane.h"
#include "event_notifier.h"
#include "drawingutils.h"
#include <wx/listbox.h>
#include "wxcl_log_text_ctrl.h"
#include <wx/listctrl.h>
#include <wx/aui/framemanager.h>
#include "cl_aui_tb_are.h"
#include "globals.h"

#ifdef __WXMAC__
#include <wx/srchctrl.h>
#endif

#define CHECK_POINTER(p) \
    if(!p) return;

ThemeHandler::ThemeHandler()
{
    EventNotifier::Get()->Connect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ThemeHandler::OnEditorThemeChanged), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(ThemeHandler::OnInitDone), NULL, this);
}

ThemeHandler::~ThemeHandler()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(ThemeHandler::OnEditorThemeChanged), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(ThemeHandler::OnInitDone), NULL, this);
}

void ThemeHandler::OnEditorThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    wxColour bgColour = EditorConfigST::Get()->GetCurrentOutputviewBgColour();
    wxColour fgColour = EditorConfigST::Get()->GetCurrentOutputviewFgColour();

    if(!bgColour.IsOk() || !fgColour.IsOk()) {
        return;
    }

    size_t pageCount = clMainFrame::Get()->GetWorkspacePane()->GetNotebook()->GetPageCount();
    for(size_t i = 0; i < pageCount; ++i) {

        wxWindow* page = clMainFrame::Get()->GetWorkspacePane()->GetNotebook()->GetPage(i);
        if(page) {
            DoUpdateColours(page, bgColour, fgColour);
        }
    }

    pageCount = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPageCount();
    for(size_t i = 0; i < pageCount; ++i) {

        wxWindow* page = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(i);
        if(page) {
            DoUpdateColours(page, bgColour, fgColour);
        }
    }

    pageCount = clMainFrame::Get()->GetDebuggerPane()->GetNotebook()->GetPageCount();
    for(size_t i = 0; i < pageCount; ++i) {

        wxWindow* page = clMainFrame::Get()->GetDebuggerPane()->GetNotebook()->GetPage(i);
        if(page) {
            DoUpdateColours(page, bgColour, fgColour);
        }
    }

    wxclTextCtrl* log = dynamic_cast<wxclTextCtrl*>(wxLog::GetActiveTarget());
    if(log) {
        log->Reset();
    }

    /// go over all the aui-toolbars and make sure they all have
    /// our custom aui artproivder
    DoUpdateAuiToolBars(clMainFrame::Get());
}

void ThemeHandler::DoUpdateColours(wxWindow* win, const wxColour& bg, const wxColour& fg)
{
#ifdef __WXMAC__
    if(dynamic_cast<wxSearchCtrl*>(win)) {
        // On OSX, this looks bad...
        // so dont alter its colours, so we simply do nothing here
    } else
#endif
        if(dynamic_cast<wxTreeCtrl*>(win) || dynamic_cast<wxListBox*>(win) || dynamic_cast<wxDataViewCtrl*>(win) ||
            dynamic_cast<wxTextCtrl*>(win) || dynamic_cast<wxListCtrl*>(win)) {
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

void ThemeHandler::DoUpdateSTCBgColour(wxStyledTextCtrl* stc)
{
    CHECK_POINTER(stc);
    for(int i = 0; i <= wxSTC_STYLE_DEFAULT; ++i) {
        stc->StyleSetBackground(i, DrawingUtils::GetOutputPaneBgColour());
    }
    stc->Refresh();
}

void ThemeHandler::OnInitDone(wxCommandEvent& e)
{
    e.Skip();
    // Load all wxAuiToolBars and make sure they all have our custom wxAuiTabArt (which supports dark theme)
    wxAuiManager& aui = clMainFrame::Get()->GetDockingManager();
    wxAuiPaneInfoArray& allPanes = aui.GetAllPanes();
    for(size_t i = 0; i < allPanes.GetCount(); ++i) {
        if(allPanes.Item(i).IsToolbar()) {
            wxAuiPaneInfo& pane = allPanes.Item(i);
            wxAuiToolBar* auibar = dynamic_cast<wxAuiToolBar*>(pane.window);
            if(auibar) {
                CLMainAuiTBArt* clArtProvider = dynamic_cast<CLMainAuiTBArt*>(auibar->GetArtProvider());
                if(!clArtProvider) {
                    /// not codelite's custom art provider
                    /// replace it
                    auibar->SetArtProvider(new CLMainAuiTBArt());
                }
            }
        }
    }
    aui.Update();
    m_helper.reset(new ThemeHandlerHelper(clMainFrame::Get()));
    // Fire "wxEVT_EDITOR_SETTINGS_CHANGED" to ensure that the notebook appearance is in sync with the settings
    PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
}

void ThemeHandler::DoUpdateAuiToolBars(wxWindow* win)
{
    // bool bDarkBG = DrawingUtils::IsDark(EditorConfigST::Get()->GetCurrentOutputviewBgColour());
    //
    // wxAuiToolBar *auibar = dynamic_cast<wxAuiToolBar*>( win );
    // if( auibar ) {
    //     CLMainAuiTBArt *clArtProvider = dynamic_cast<CLMainAuiTBArt*>(auibar->GetArtProvider());
    //     if ( !clArtProvider ) {
    //         /// not codelite's custom art provider
    //         /// replace it
    //         auibar->SetArtProvider( new CLMainAuiTBArt() );
    //     }
    //     size_t toolCount = auibar->GetToolCount();
    //     for(size_t i=0; i<toolCount; i++) {
    //         wxAuiToolBarItem* tool = auibar->FindToolByIndex(i);
    //         if ( tool->GetKind() == wxITEM_NORMAL || tool->GetKind() == wxITEM_CHECK || tool->GetKind() ==
    //         wxITEM_DROPDOWN || tool->GetKind() == wxITEM_RADIO ) {
    //             tool->SetDisabledBitmap( tool->GetBitmap().ConvertToDisabled(bDarkBG ? 50 : 255) );
    //         }
    //     }
    //     auibar->Refresh();
    // }
    //
    // wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
    // while(pclNode) {
    //     wxWindow* pclChild = pclNode->GetData();
    //     this->DoUpdateAuiToolBars(pclChild);
    //     pclNode = pclNode->GetNext();
    // }
}
