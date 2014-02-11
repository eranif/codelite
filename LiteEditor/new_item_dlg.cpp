//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : new_item_dlg.cpp
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
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "new_item_dlg.h"
#include "pluginmanager.h"
#include "windowattrmanager.h"
#include "globals.h"
#include "wx/xrc/xmlres.h"
#include "wx/textctrl.h"
#include "dirsaver.h"
#include "macros.h"
#include <wx/imaglist.h>

static const wxString FileTypeCpp = wxT("C++ Source File (.cpp)");
static const wxString FileTypeC = wxT("C Source File (.c)");
static const wxString FileTypeHeader = wxT("Header File (.h)");
static const wxString FileTypeAny = wxT("Any File");

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(NewItemDlg, NewItemBaseDlg)
    EVT_CHAR_HOOK(NewItemDlg::OnCharHook)
END_EVENT_TABLE()

NewItemDlg::NewItemDlg( wxWindow* parent, wxString cwd)
    : NewItemBaseDlg(parent)
{
    m_cwd = cwd;
    m_fileType->InsertColumn(0, _("File Type"));
    m_fileType->SetColumnWidth(0, 300);

    // Initialise images map
    wxImageList *images = new wxImageList(16, 16, true);
    BitmapLoader *bmpLoader = PluginManager::Get()->GetStdIcons();
    images->Add(bmpLoader->LoadBitmap(wxT("mime/16/c")));     //0
    images->Add(bmpLoader->LoadBitmap(wxT("mime/16/cpp")));   //1
    images->Add(bmpLoader->LoadBitmap(wxT("mime/16/h")));     //2
    images->Add(bmpLoader->LoadBitmap(wxT("mime/16/text")));  //3

    m_fileType->AssignImageList( images,  wxIMAGE_LIST_SMALL );

    //-----------------------------------
    // Populate the list:
    m_fileTypeValue = FileTypeCpp;

    long row = AppendListCtrlRow(m_fileType);
    SetColumnText(m_fileType, row, 0, FileTypeCpp, 1);

    row = AppendListCtrlRow(m_fileType);
    SetColumnText(m_fileType, row, 0, FileTypeC, 0);

    row = AppendListCtrlRow(m_fileType);
    SetColumnText(m_fileType, row, 0, FileTypeHeader, 2);

    row = AppendListCtrlRow(m_fileType);
    SetColumnText(m_fileType, row, 0, FileTypeAny, 3);
    m_fileTypeValue = FileTypeAny;

    //select the last item
    m_fileType->SetItemState(row, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    m_fileType->SetItemState(row, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);

    m_location->SetValue(m_cwd);
    m_fileName->SetFocus();

    // Attach events
    ConnectEvents();
    WindowAttrManager::Load(this, wxT("NewItemDlg"), NULL);
}

void NewItemDlg::ConnectEvents()
{
    m_cancel->Connect(m_cancel->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewItemDlg::OnClick), NULL, this);
    m_okButton->Connect(m_okButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewItemDlg::OnClick), NULL, this);
    m_browseBtn->Connect(m_browseBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewItemDlg::OnClick), NULL, this);
    m_fileType->Connect(m_fileType->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(NewItemDlg::OnListItemSelected), NULL, this);
}


void NewItemDlg::OnClick(wxCommandEvent &event)
{
    int id = event.GetId();
    if ( id == m_okButton->GetId() ) {
        DoCreateFile();
    } else if ( id == m_cancel->GetId() ) {
        EndModal(wxID_CANCEL);
    } else if ( id == m_browseBtn->GetId() ) {
        DirSaver ds;
        wxDirDialog *dlg = new wxDirDialog(this, _("Location:"),  m_cwd);
        if (dlg->ShowModal() == wxID_OK) {
            m_location->SetValue(dlg->GetPath());
        }
        dlg->Destroy();
    }
}

void NewItemDlg::DoCreateFile()
{
    wxString errMsg;
    if ( !Validate(errMsg) ) {
        wxMessageBox(errMsg, _("CodeLite"), wxICON_INFORMATION | wxOK);
        return;
    }

    // Construct the file name
    wxString fileName(m_fileName->GetValue());
    TrimString(fileName);

    if (m_fileTypeValue == FileTypeAny) {
        m_newFileName = wxFileName(m_location->GetValue(), fileName);
        
    } else if ( m_fileTypeValue == FileTypeC ) {
        // If user already provided suffix, dont add another one on top of it
        if (fileName.Find(wxT(".")) == wxNOT_FOUND ) {
            m_newFileName = wxFileName(m_location->GetValue(), fileName, wxT("c"));
        } else {
            m_newFileName = wxFileName(m_location->GetValue(), fileName);
        }
        
    } else if ( m_fileTypeValue == FileTypeCpp ) {
        // If user already provided suffix, dont add another one on top of it
        if (fileName.Find(wxT(".")) == wxNOT_FOUND ) {
            m_newFileName = wxFileName(m_location->GetValue(), fileName, wxT("cpp"));
        } else {
            m_newFileName = wxFileName(m_location->GetValue(), fileName);
        }
        
    } else if ( m_fileTypeValue == FileTypeHeader ) {
        // If user already provided suffix, dont add another one on top of it
        if (fileName.Find(wxT(".")) == wxNOT_FOUND ) {
            m_newFileName = wxFileName(m_location->GetValue(), fileName, wxT("h"));
        } else {
            m_newFileName = wxFileName(m_location->GetValue(), fileName);
        }
    }
    EndModal(wxID_OK);
}

bool NewItemDlg::Validate(wxString &errMsg)
{
    // make sure we have file name & path set up correctly
    wxFileName fn(m_location->GetValue() + wxFileName::GetPathSeparator());

    if ( m_location->GetValue().Trim().IsEmpty() ) {
        errMsg = _("Missing location");
        return false;
    }

    fn = wxFileName(m_location->GetValue(), m_fileName->GetValue());
    if ( fn.FileExists() ) {
        errMsg = _("A file with that name already exists. Please choose a different name");
        return false;
    }

    if ( m_fileName->GetValue().Trim().IsEmpty() ) {
        errMsg = _("Missing file name");
        return false;
    }

    return true;
}

void NewItemDlg::OnListItemSelected(wxListEvent &event)
{
    m_fileTypeValue = event.GetText();
}

void NewItemDlg::OnCharHook(wxKeyEvent &event)
{
    if (event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
        DoCreateFile();
    }
    event.Skip();
}

NewItemDlg::~NewItemDlg()
{
    WindowAttrManager::Save(this, wxT("NewItemDlg"), NULL);
}
