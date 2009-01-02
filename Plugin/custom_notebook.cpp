//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : custom_notebook.cpp
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
#include "notebooknavdialog.h"
#include "custom_notebook.h"
#include "custom_tab.h"
#include "custom_tabcontainer.h"
#include "wx/sizer.h"


const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGED  = XRCID("notebook_page_changing");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGING = XRCID("notebook_page_changed");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSING  = XRCID("notebook_page_closing");
const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSED   = XRCID("notebook_page_closed");

BEGIN_EVENT_TABLE(Notebook, wxPanel)
	EVT_NAVIGATION_KEY(Notebook::OnNavigationKey)
END_EVENT_TABLE()

Notebook::Notebook(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
		: wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
		, m_style(style)
		, m_aui(NULL)
		, m_popupWin(NULL)
{
	Initialize();
}

Notebook::~Notebook()
{
}

void Notebook::AddPage(wxWindow *win, const wxString &text, const wxString &tooltip, const wxBitmap &bmp, bool selected)
{
	Freeze();
	CustomTab *tab = new CustomTab(m_tabs, wxID_ANY, text, tooltip, bmp, selected, m_tabs->GetOrientation(), m_style);
	win->Reparent(this);
	tab->SetWindow(win);

	AddPage(tab);
	GetSizer()->Layout();

	Thaw();
}

void Notebook::Initialize()
{
	wxBoxSizer *sz = NULL;


	int ori(wxRIGHT);
	if (m_style & wxVB_LEFT) {
		ori = wxLEFT;
		sz = new wxBoxSizer(wxHORIZONTAL);
	} else if (m_style & wxVB_TOP) {
		ori = wxTOP;
		sz = new wxBoxSizer(wxVERTICAL);
	} else if (m_style & wxVB_BOTTOM) {
		ori = wxBOTTOM;
		sz = new wxBoxSizer(wxVERTICAL);
	} else {
		sz = new wxBoxSizer(wxHORIZONTAL);
	}

	SetSizer(sz);

	m_tabs = new wxTabContainer(this, wxID_ANY, ori, m_style);
	sz->Add(m_tabs, 0, wxEXPAND);
	sz->Layout();
}

void Notebook::SetSelection(size_t page)
{
	CustomTab *tab = m_tabs->IndexToTab(page);
	CustomTab *old_tab = m_tabs->GetSelection();

	if (old_tab == tab) {
		//same tab, nothing to be done
		return;
	}

	if (tab) {
		tab->GetWindow()->SetFocus();

		//the next call will also trigger a call to Notebook::SetSelection(CustomTab *tab)
		m_tabs->SetSelection(tab, true);
	}
}

//this function is called from the wxTabContainer class
void Notebook::SetSelection(CustomTab *tab)
{
	//the tab control is already updated, all left to be done is to set the window
	wxSizer *sz = GetSizer();
	wxWindow *oldWindow(NULL);
	CustomTab *oldSelection = m_tabs->GetSelection();
	if (oldSelection) {
		oldWindow = oldSelection->GetWindow();
	}


	wxWindow *win = tab->GetWindow();
	if (oldWindow == win) {
		//nothing to be done
		return;
	}

	Freeze();
	if (m_style & wxVB_LEFT || m_style & wxVB_TOP) {
		sz->Insert(1, win, 1, wxEXPAND);
	} else {
		sz->Insert(0, win, 1, wxEXPAND);
	}
	win->Show();

	if (oldWindow && sz->GetItem(oldWindow)) {
		//the item indeed exist in the sizer, remove it
		sz->Detach(oldWindow);
		oldWindow->Hide();
	}

	sz->Layout();
	Thaw();
}

size_t Notebook::GetSelection()
{
	CustomTab *tab = m_tabs->GetSelection();
	if (tab) {
		return m_tabs->TabToIndex(tab);
	}
	return Notebook::npos;
}

wxWindow* Notebook::GetPage(size_t page)
{
	CustomTab *tab = m_tabs->IndexToTab(page);
	if (tab) {
		return tab->GetWindow();
	}
	return NULL;
}

size_t Notebook::GetPageCount() const
{
	return m_tabs->GetTabsCount();
}

bool Notebook::RemovePage(size_t page, bool notify)
{
    bool res = false;
	Freeze();
	CustomTab *tab = m_tabs->IndexToTab(page);
	if (tab) {
		res = m_tabs->RemovePage(tab, notify);
	}
	Thaw();
    return res;

}

bool Notebook::DeletePage(size_t page, bool notify)
{
    bool res = false;
	Freeze();
	CustomTab *tab = m_tabs->IndexToTab(page);
	if (tab) {
		res = m_tabs->DeletePage(tab, notify);
	}
	Thaw();
    return res;
}

wxString Notebook::GetPageText(size_t page) const
{
	CustomTab *tab = m_tabs->IndexToTab(page);
	if (tab) {
		return tab->GetText();
	}
	return wxEmptyString;
}

void Notebook::SetOrientation(int orientation)
{
	int add_style(orientation);

	wxSizer *sz = GetSizer();
	m_style &= ~(wxVB_LEFT | wxVB_RIGHT | wxVB_TOP | wxVB_BOTTOM);
	m_style |= add_style;

	int ori(wxRIGHT);
	if (m_style & wxVB_LEFT) {
		ori = wxLEFT;
	} else if (m_style & wxVB_TOP) {
		ori = wxTOP;
	} else if (m_style & wxVB_BOTTOM) {
		ori = wxBOTTOM;
	}
	m_tabs->SetOrientation(ori);

	//detach the tabcontainer class from the sizer
	if (GetPageCount() > 0) {
		sz->Detach(m_tabs);
		if (m_style & wxVB_LEFT || m_style & wxVB_TOP) {
			sz->Insert(0, m_tabs, 0, wxEXPAND);
		} else {
			sz->Add(m_tabs, 0, wxEXPAND);
		}

	}

	m_tabs->Resize();
	sz->Layout();
}

void Notebook::SetAuiManager(wxAuiManager *manager, const wxString &containedPaneName)
{
	if (manager) {
		m_aui = manager;
		m_paneName = containedPaneName;
		m_aui->Connect(wxEVT_AUI_RENDER, wxAuiManagerEventHandler(Notebook::OnRender), NULL, this);
	} else {
		if (m_aui) {
			m_aui->Disconnect(wxEVT_AUI_RENDER, wxAuiManagerEventHandler(Notebook::OnRender), NULL, this);
		}
		m_aui = NULL;
		m_paneName = wxEmptyString;
	}
}

void Notebook::OnRender(wxAuiManagerEvent &e)
{
	if (m_aui) {
		wxAuiPaneInfo info = m_aui->GetPane( m_paneName );
		if (info.IsOk()) {
			//we got the containing pane of the book, test its orientation
			if (info.dock_direction == wxAUI_DOCK_LEFT && m_style & wxVB_RIGHT) {
				SetOrientation(wxVB_LEFT);
			} else if (info.dock_direction == wxAUI_DOCK_RIGHT && m_style & wxVB_LEFT) {
				SetOrientation(wxVB_RIGHT);
			}
		}
	}
	e.Skip();
}

void Notebook::OnNavigationKey(wxNavigationKeyEvent &e)
{
	CustomTab *tab(NULL);
	if ( e.IsWindowChange() ) {
		if ( !m_popupWin && GetPageCount() > 0) {
			m_popupWin = new NotebookNavDialog( this );
			if(m_popupWin->ShowModal() == wxID_OK && m_popupWin->GetSelection()){
				tab = m_popupWin->GetSelection();
				size_t idx = m_tabs->TabToIndex(tab);
				SetSelection(idx);
			}

			m_popupWin->Destroy();
			m_popupWin = NULL;

			if(tab) {
				tab->GetWindow()->SetFocus();
			}

		} else if ( m_popupWin ) {
			// a dialog is already opened
			m_popupWin->OnNavigationKey( e );
			return;
		}
	} else {
		// pass to the parent
		if ( GetParent() ) {
			e.SetCurrentFocus(this);
			GetParent()->ProcessEvent(e);
		}
	}
}

const wxArrayPtrVoid& Notebook::GetHistory() const
{
	return m_tabs->GetHistory();
}

void Notebook::AddPage(CustomTab *tab)
{
	wxWindow *oldWindow(NULL);
	CustomTab *oldSelection = m_tabs->GetSelection();
	if (oldSelection) {
		oldWindow = oldSelection->GetWindow();
	}

	//add the tab
	m_tabs->AddTab(tab);

	//add the actual window to the book sizer
	wxSizer *sz = GetSizer();
	wxWindow *win = tab->GetWindow();


	if (tab->GetSelected()) {
		//inert the new item
		if (m_style & wxVB_LEFT || m_style & wxVB_TOP) {
			sz->Insert(1, win, 1, wxEXPAND);
		} else {
			sz->Insert(0, win, 1, wxEXPAND);
		}

		if (oldWindow && sz->GetItem(oldWindow)) {
			//the item indeed exist in the sizer, remove it
			sz->Detach(oldWindow);
			oldWindow->Hide();
		}
	}else{
		win->Hide();
	}
}

void Notebook::SetRightClickMenu(wxMenu* menu)
{
	m_tabs->SetRightClickMenu( menu );
}

wxWindow* Notebook::GetCurrentPage()
{
	CustomTab *tab =  m_tabs->GetSelection();
	if(tab) {
		return tab->GetWindow();
	}
	return NULL;
}

size_t Notebook::GetPageIndex(wxWindow *page)
{
	for(size_t i=0; i< m_tabs->GetTabsCount(); i++) {
		CustomTab *tab = m_tabs->IndexToTab(i);
		if(tab->GetWindow() == page) {
			return i;
		}
	}
	return Notebook::npos;
}

size_t Notebook::GetPageIndex(const wxString& text)
{
	for(size_t i=0; i< m_tabs->GetTabsCount(); i++) {
		CustomTab *tab = m_tabs->IndexToTab(i);
		if(tab->GetText() == text) {
			return i;
		}
	}
	return Notebook::npos;
}

void Notebook::SetPageText(size_t index, const wxString &text)
{
	CustomTab *tab = m_tabs->IndexToTab(index);
	if(tab) {
		tab->SetText(text);
		tab->Refresh();

		//this requires re-calculating the tabs are
		m_tabs->Resize();
	}
}

bool Notebook::DeleteAllPages(bool notify)
{
    bool res = true;
	Freeze();

	size_t count = m_tabs->GetTabsCount();
	for(size_t i=0; i<count && res; i++){
		res = DeletePage(0, notify);
	}

	Thaw();
    return res;
}

