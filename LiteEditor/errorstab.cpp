//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : buidltab.cpp
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
#include "errorstab.h"
#include "drawingutils.h"
#include "findresultstab.h"
#include "pluginmanager.h"
#include "treelistctrl.h"
#include "fileextmanager.h"
#include "globals.h"
#include "editor_config.h"

class ErrorsTabItemData : public wxTreeItemData
{
public:
	BuildTab::LineInfo m_lineInfo;
	
public:
	ErrorsTabItemData(const BuildTab::LineInfo& lineInfo) {
		m_lineInfo = lineInfo;
	}
	
	virtual ~ErrorsTabItemData(){}
};

ErrorsTab::ErrorsTab ( BuildTab *bt, wxWindow *parent, wxWindowID id, const wxString &name )
		: OutputTabWindow ( parent, id, name )
		, m_bt(bt)
		, m_errorCount(0)
		, m_warningCount(0)
{
    m_autoAppear = false; // BuildTab controls this tab's auto-appearance
	m_tb->RemoveTool( XRCID("repeat_output"   ));
	m_tb->RemoveTool( XRCID("search_output"   ));
	m_tb->RemoveTool( XRCID("word_wrap_output"));
	m_tb->RemoveTool( XRCID("word_wrap_output"));
	m_tb->RemoveTool( XRCID("collapse_all"));
	m_tb->Realize();
	
	m_hSizer->Detach(m_sci);
	m_sci->Hide();
	
	// Insert a wxTreeListCtrl 
	long treeStyle = wxTR_HIDE_ROOT|wxTR_COLUMN_LINES|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HAS_BUTTONS|wxTR_NO_LINES|wxTR_ROW_LINES;
	m_treeListCtrl = new wxTreeListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, treeStyle);
	MSWSetNativeTheme(m_treeListCtrl);
	
	m_treeListCtrl->AddColumn(_("File" ), 800);
	m_treeListCtrl->AddColumn(_("Line") , 800);
	
	wxImageList *imageList = new wxImageList(16, 16, true);
	imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("status/16/error")));                        // 0
	imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("status/16/warning")));                      // 1
	imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("mime/16/c")));                              // 2
	imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("mime/16/cpp")));                            // 3
	imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("mime/16/h")));                              // 4
	imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("mime/16/text")));                           // 5
	imageList->Add(PluginManager::Get()->GetStdIcons()->LoadBitmap(wxT("toolbars/16/unittest++/run_as_unittest"))); // 6
	
	m_treeListCtrl->AssignImageList( imageList );
	m_treeListCtrl->AddRoot(_("Build Output"));
	m_treeListCtrl->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(ErrorsTab::OnItemDClick), NULL, this);
#ifdef __WXMAC__
	m_hSizer->Insert(0, m_treeListCtrl, 1, wxEXPAND);
#else
	m_hSizer->Add(m_treeListCtrl, 1, wxEXPAND);
#endif
	wxTheApp->Connect ( wxEVT_BUILD_ENDED , wxCommandEventHandler ( ErrorsTab::OnBuildEnded   ), NULL, this );
}

ErrorsTab::~ErrorsTab()
{
}

void ErrorsTab::ClearLines()
{
	m_treeListCtrl->DeleteChildren(m_treeListCtrl->GetRootItem());
	m_errorCount = 0;
	m_warningCount = 0;
}

void ErrorsTab::AddError ( const BuildTab::LineInfo &lineInfo )
{
	if(lineInfo.linecolor != wxSCI_LEX_GCC_ERROR && lineInfo.linecolor != wxSCI_LEX_GCC_WARNING)
		return;
	
	bool isError = (lineInfo.linecolor == wxSCI_LEX_GCC_ERROR);
	isError ? m_errorCount++ : m_warningCount++;
	
	wxTreeItemId item = DoFindFile(lineInfo.filename);
	if(!item.IsOk()) {
		int imgId(5); // text
		switch(FileExtManager::GetType(lineInfo.filename)) {
		case FileExtManager::TypeHeader:
			imgId = 4;
			break;
		
		case FileExtManager::TypeSourceC:
			imgId = 2;
			break;
			
		case FileExtManager::TypeSourceCpp:
			imgId = 3;
			break;
		
		default:
			imgId = 5;
			break;
		}
		
		// append new entry
		item = m_treeListCtrl->AppendItem(m_treeListCtrl->GetRootItem(), lineInfo.filename, imgId, imgId);
		wxColour rootItemColour = DrawingUtils::LightColour(wxT("LIGHT GRAY"), 3.0);
		m_treeListCtrl->SetItemBackgroundColour(item, rootItemColour);
	}
	
	// Dont add duplicate entries
	if(IsMessageExists(lineInfo.linetext, item))
		return;
		
	wxString displayText = lineInfo.linetext.Mid(lineInfo.filestart + lineInfo.filelen);
	
	if(m_tb->GetToolState(XRCID("scroll_on_output"))) {
		// Make sure the new entry is visible
		m_treeListCtrl->EnsureVisible(item);
	}
	
	wxTreeItemId newItem = m_treeListCtrl->AppendItem(item, displayText, isError ? 0 : 1, isError ? 0 : 1, new ErrorsTabItemData(lineInfo));
	m_treeListCtrl->SetItemText(newItem, 1, wxString::Format(wxT("%ld"), lineInfo.linenum + 1));
}

void ErrorsTab::OnClearAll ( wxCommandEvent& e )
{
	m_bt->OnClearAll ( e );
}

void ErrorsTab::OnClearAllUI ( wxUpdateUIEvent& e )
{
	m_bt->OnClearAllUI ( e );
}

void ErrorsTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
	if(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {
		e.Enable(true);
		e.Check( EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfErrors() );
		
	} else {
		e.Enable(false);
		e.Check(false);
	}
}

wxTreeItemId ErrorsTab::DoFindFile(const wxString& filename)
{
	wxTreeItemIdValue cookieOne;
	wxTreeItemId child = m_treeListCtrl->GetFirstChild(m_treeListCtrl->GetRootItem(), cookieOne);
	while( child.IsOk() ) {
		if(m_treeListCtrl->GetItemText(child) == filename)
			return child;
		child = m_treeListCtrl->GetNextChild(m_treeListCtrl->GetRootItem(), cookieOne);
	}
	return wxTreeItemId();
}

void ErrorsTab::OnItemDClick(wxTreeEvent& event)
{
	if(event.GetItem().IsOk() == false) {
		return;
	}
	
	ErrorsTabItemData *itemData = dynamic_cast<ErrorsTabItemData*>(m_treeListCtrl->GetItemData(event.GetItem()));
	if(itemData) {
		m_bt->DoOpenFile(itemData->m_lineInfo);
	}
	event.Skip();
}

bool ErrorsTab::IsMessageExists(const wxString& msg, const wxTreeItemId& item)
{
	wxTreeItemIdValue cookieOne;
	wxTreeItemId child = m_treeListCtrl->GetFirstChild(item, cookieOne);
	while( child.IsOk() ) {
		ErrorsTabItemData *itemData = (ErrorsTabItemData*)(m_treeListCtrl->GetItemData(item));
		if(itemData && itemData->m_lineInfo.linetext == msg)
			return true;
		child = m_treeListCtrl->GetNextChild(item, cookieOne);
	}
	return false;
}

void ErrorsTab::OnBuildEnded(wxCommandEvent& event)
{
	// Count the number of errors
	if(m_warningCount == 0 && m_errorCount == 0) {
		// No errors were found!
		m_treeListCtrl->AppendItem(m_treeListCtrl->GetRootItem(), _("Build ended successfully."), 6, 6);
		
	} else {
		wxTreeItemIdValue cookieTwo;
		wxTreeItemId firstFile = m_treeListCtrl->GetFirstChild(m_treeListCtrl->GetRootItem(), cookieTwo);
		if(firstFile.IsOk()) {
			m_treeListCtrl->Expand(firstFile);
		}
		
		// Add status mesage
		wxString msg;
		int msgIcon(0); // error icon
		if(m_errorCount == 0) {
			msg = wxString::Format(_("Build ended with %d warnings"), m_warningCount);
			msgIcon = 1;
			
		} else if(m_warningCount == 0) {
			msg = wxString::Format(_("Build ended with %d errors"), m_errorCount);
			msgIcon = 0;
			
		} else {
			msg = wxString::Format(_("Build ended with %d errors, %d warnings"), m_errorCount, m_warningCount);
			msgIcon = 0;
		}
		
		wxTreeItemId statusItem = m_treeListCtrl->InsertItem(m_treeListCtrl->GetRootItem(), 0, msg, msgIcon, msgIcon);
		m_treeListCtrl->SetItemBold(statusItem);
		m_treeListCtrl->EnsureVisible( statusItem );
	}
}
