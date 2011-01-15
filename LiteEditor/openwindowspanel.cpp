//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : openwindowspanel.cpp
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
#include <wx/xrc/xmlres.h>
#include "globals.h"
#include <wx/clntdata.h>
#include "frame.h"
#include "manager.h"
#include "openwindowspanel.h"
#include "string_client_data.h"

BEGIN_EVENT_TABLE(OpenWindowsPanel, OpenWindowsPanelBase)
EVT_MENU(XRCID("wxID_CLOSE_SELECTED"), OpenWindowsPanel::OnCloseSelectedFiles)
EVT_MENU(XRCID("wxID_SAVE_SELECTED"), OpenWindowsPanel::OnSaveSelectedFiles)
END_EVENT_TABLE()

OpenWindowsPanel::OpenWindowsPanel( wxWindow* parent, const wxString &caption )
    : OpenWindowsPanelBase( parent )
    , m_caption(caption)
    , m_rclickMenu(wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click")))
    , m_mutliMenu(wxXmlResource::Get()->LoadMenu(wxT("tabs_multi_sels_menu")))
{
	MSWSetNativeTheme(m_fileList);
	
    wxTheApp->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OpenWindowsPanel::OnActiveEditorChanged), NULL, this);
    wxTheApp->Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(OpenWindowsPanel::OnEditorClosing), NULL, this);
    wxTheApp->Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OpenWindowsPanel::OnAllEditorsClosed), NULL, this);
}

OpenWindowsPanel::~OpenWindowsPanel()
{
	if (m_rclickMenu) {
		delete m_rclickMenu;
		m_rclickMenu = NULL;
	}
	
	if(m_mutliMenu) {
		delete m_mutliMenu;
		m_mutliMenu = NULL;
	}
	
    // clear list now, or wxGTK seems to crash on exit
    m_fileList->Clear();
	
    wxTheApp->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(OpenWindowsPanel::OnActiveEditorChanged), NULL, this);
    wxTheApp->Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(OpenWindowsPanel::OnEditorClosing), NULL, this);
    wxTheApp->Disconnect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(OpenWindowsPanel::OnAllEditorsClosed), NULL, this);
}

int OpenWindowsPanel::EditorItem(LEditor *editor)
{
    if (editor) {
        wxString path = editor->GetFileName().GetFullPath();
        for (unsigned i = 0; i < m_fileList->GetCount(); i++) {
            MyStringClientData *data = dynamic_cast<MyStringClientData *>(m_fileList->GetClientObject(i));
            if (data->GetData() == path)
                return i;
        }
    }
    return wxNOT_FOUND;
}

void OpenWindowsPanel::DoOpenSelectedItem(int item)
{
	MyStringClientData *data = dynamic_cast<MyStringClientData *>(m_fileList->GetClientObject(item));
    clMainFrame::Get()->GetMainBook()->OpenFile(data->GetData(), wxEmptyString);
}

void OpenWindowsPanel::DoCloseSelectedItem(int item)
{
    DoOpenSelectedItem(item); // make sure the editor is selected in MainBook
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_file"));
    AddPendingEvent(e);
}

void OpenWindowsPanel::OnKeyDown( wxKeyEvent& event )
{
    switch (event.GetKeyCode()) {
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
        case WXK_SPACE:
            if (m_fileList->GetSelection() != wxNOT_FOUND) {
                DoOpenSelectedItem(m_fileList->GetSelection());
            }
            break;
        case WXK_DELETE:
        case WXK_NUMPAD_DELETE:
            if (m_fileList->GetSelection() != wxNOT_FOUND) {
                DoCloseSelectedItem(m_fileList->GetSelection());
                m_fileList->SetFocus();
            }
            break;
        default:
            event.Skip();
            break;
    }
}

void OpenWindowsPanel::OnItemDClicked( wxCommandEvent& event )
{
	DoOpenSelectedItem(event.GetSelection());
}

void OpenWindowsPanel::OnRightDown( wxMouseEvent& event )
{
	wxUnusedVar(event);
	wxArrayInt sels;
    m_fileList->GetSelections(sels);
	
	if(sels.GetCount() == 1) {
        DoOpenSelectedItem(sels.Item(0));
        PopupMenu(m_rclickMenu);
    } else {
		PopupMenu(m_mutliMenu);
	}
}

void OpenWindowsPanel::OnChar(wxKeyEvent& event)
{
    OnKeyDown(event);
}

void OpenWindowsPanel::OnActiveEditorChanged(wxCommandEvent& e)
{
    e.Skip();
    LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
	if(!editor)
		return;

    int i = EditorItem(editor);
    if (i != wxNOT_FOUND && i == m_fileList->GetSelection())
        return;

    if (i == wxNOT_FOUND) {
        wxString txt = editor->GetFileName().GetFullName();
        MyStringClientData *data = new MyStringClientData(editor->GetFileName().GetFullPath());
        i = m_fileList->Append(txt, data);
    }
    m_fileList->Select(i);
    m_fileList->EnsureVisible(i);
}

void OpenWindowsPanel::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();
    m_fileList->Clear();
}

void OpenWindowsPanel::OnEditorClosing(wxCommandEvent& e)
{
    e.Skip();
    LEditor *editor = dynamic_cast<LEditor*>((IEditor*) e.GetClientData());
	if(!editor)
		return;

    int i = EditorItem(editor);
    if (i != wxNOT_FOUND) {
        m_fileList->Delete(i);
    }
}

void OpenWindowsPanel::OnCloseSelectedFiles(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxArrayInt sels;
    m_fileList->GetSelections(sels);
	if(sels.IsEmpty())
		return;
		
	for(int i=0; i<(int)sels.GetCount(); i++) {
		DoCloseSelectedItem(sels.Item(i));
	}
}

void OpenWindowsPanel::OnSaveSelectedFiles(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxArrayInt sels;
    m_fileList->GetSelections(sels);
	if(sels.IsEmpty())
		return;
		
	for(int i=0; i<(int)sels.GetCount(); i++) {
		DoSaveItem(sels.Item(i));
	}
}

void OpenWindowsPanel::DoSaveItem(int item)
{
	MyStringClientData *data = dynamic_cast<MyStringClientData *>(m_fileList->GetClientObject(item));
	if(data) {
		LEditor * editor = clMainFrame::Get()->GetMainBook()->FindEditor(data->GetData());
		if(editor) {
			editor->SaveFile();
		}
	}
}
