//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dockablepane.cpp
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

#include "Notebook.h"
#include "clSystemSettings.h"
#include "dockablepane.h"
#include "globals.h"
#include "imanager.h"
#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/xrc/xmlres.h>

const wxEventType wxEVT_CMD_NEW_DOCKPANE = XRCID("new_dockpane");
const wxEventType wxEVT_CMD_DELETE_DOCKPANE = XRCID("delete_dockpane");

BEGIN_EVENT_TABLE(DockablePane, wxPanel)
EVT_ERASE_BACKGROUND(DockablePane::OnEraseBg)
EVT_PAINT(DockablePane::OnPaint)
END_EVENT_TABLE()

DockablePane::DockablePane(wxWindow* parent, Notebook* book, const wxString& title, bool initialFloat,
                           const wxBitmap& bmp, wxSize size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxBORDER_NONE)
    , m_child(NULL)
    , m_book(book)
    , m_text(title)
    , m_bmp(bmp)
    , m_notifiedDestroyed(false)
{
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);

    Connect(XRCID("close_pane"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DockablePane::ClosePane));
    wxAuiPaneInfo info;
    info.Name(title).Caption(title);
    if(initialFloat) { info.Float(); }
    clGetManager()->GetDockingManager()->AddPane(this, info);
    clGetManager()->GetDockingManager()->Update();

    wxCommandEvent event(wxEVT_CMD_NEW_DOCKPANE);
    event.SetClientData(this);
    parent->GetEventHandler()->AddPendingEvent(event);
}

DockablePane::~DockablePane() {}

void DockablePane::ClosePane(wxCommandEvent& e)
{
    wxUnusedVar(e);

    if(!m_notifiedDestroyed) {
        m_notifiedDestroyed = true;

        if(m_book) {
            // first detach the child from this pane
            wxSizer* sz = GetSizer();
            sz->Detach(m_child);

            // now we can add it to the noteook (it will be automatically be reparented to the notebook)
            m_book->AddPage(m_child, m_text, false, m_bmp);
        }

        wxCommandEvent event(wxEVT_CMD_DELETE_DOCKPANE);
        event.SetClientData(this);
        GetParent()->GetEventHandler()->AddPendingEvent(event);
    }
}

void DockablePane::OnPaint(wxPaintEvent& e)
{
    wxBufferedPaintDC dc(this);
    
    wxRect rect = GetClientRect();
    rect.Inflate(1);
    dc.SetPen(wxPen(clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
    dc.SetBrush(wxBrush(clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
    dc.DrawRectangle(rect);
}

void DockablePane::SetChildNoReparent(wxWindow* child)
{
    m_child = child;
    wxSizer* sz = GetSizer();
    if(!m_child->IsShown()) { m_child->Show(); }
    sz->Add(m_child, 1, wxEXPAND | wxALL, 0);
    sz->Layout();
}
