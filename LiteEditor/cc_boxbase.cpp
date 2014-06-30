//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : cc_boxbase.cpp
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

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cc_boxbase.h"
#include <wx/pen.h>
#include "ccboxmainpanel.h"
#if wxVERSION_NUMBER >= 2809
#include <wx/aui/auibar.h>
#else
#include <wx/toolbar.h>
#endif
#include <wx/xrc/xmlres.h>


#include "pluginmanager.h"
///////////////////////////////////////////////////////////////////////////

CCBoxBase::CCBoxBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
    : CCBoxParent( parent )
{
    SetSizeHints(BOX_WIDTH, BOX_HEIGHT);
    wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* topSizer  = new wxBoxSizer( wxHORIZONTAL );
    SetSizer(mainSizer);

    m_mainPanel = new CCBoxMainPanel(this);
    m_mainPanel->SetSizer(topSizer);
    mainSizer->Add(m_mainPanel, 1, wxEXPAND);
    m_listCtrl = new CCVirtualListCtrl( m_mainPanel,
                                        wxID_ANY,
                                        wxDefaultPosition,
                                        wxSize(BOX_WIDTH - 16, BOX_HEIGHT),
                                        wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VIRTUAL|wxBORDER_SIMPLE);
    topSizer->Add( m_listCtrl, 1, wxEXPAND|wxALL, 0);
   
    //m_mainPanel->SetAutoLayout( true );
    GetSizer()->Fit(this);

    // Connect Events
    m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CCBoxBase::OnItemActivated ), NULL, this );
    m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CCBoxBase::OnItemDeSelected ), NULL, this );
    m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CCBoxBase::OnItemSelected ), NULL, this );
    m_listCtrl->Connect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( CCBoxBase::OnKeyDown ), NULL, this );
}

CCBoxBase::~CCBoxBase()
{
    // Disconnect Events
    m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CCBoxBase::OnItemActivated ), NULL, this );
    m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CCBoxBase::OnItemDeSelected ), NULL, this );
    m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CCBoxBase::OnItemSelected ), NULL, this );
    m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( CCBoxBase::OnKeyDown ), NULL, this );
}
