//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editsnippetsdlg.cpp
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

#include "editsnippetsdlg.h"

#include "AboutHtml.h"
#include "clKeyboardManager.h"
#include "snipwiz.h"

#include <wx/msgdlg.h>

EditSnippetsDlg::EditSnippetsDlg(wxWindow* parent, SnipWiz* plugin, IManager* manager)
    : EditSnippetsBaseDlg(parent)
    , m_pPlugin(plugin)
    , m_manager(manager)
{
    Initialize();
    m_listBox1->SetFocus();
}

void EditSnippetsDlg::OnItemSelected(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString selection = m_listBox1->GetStringSelection();
    DoItemSelected(selection);
}

void EditSnippetsDlg::SelectItem(long index)
{
    m_listBox1->SetSelection(index);
    wxString snippetStr = m_listBox1->GetString((unsigned int)index);
    DoItemSelected(snippetStr);
}

void EditSnippetsDlg::DoItemSelected(const wxString& text)
{
    m_textCtrlMenuEntry->SetValue(text);
    m_textCtrlSnippet->SetValue(GetStringDb()->GetSnippetString(text));
    MenuItemDataMap_t accelMap;
    clKeyboardManager::Get()->GetAllAccelerators(accelMap);
    if(text.IsEmpty()) {
        return;
    }

    m_textCtrlAccelerator->SetValue("");
    MenuItemDataMap_t::iterator iter = accelMap.begin();
    for(; iter != accelMap.end(); ++iter) {
        MenuItemData mid = iter->second;
        if(mid.parentMenu == _("SnipWiz") && mid.action == text) {
            m_textCtrlAccelerator->SetValue(mid.accel.ToString());
        }
    }
}

void EditSnippetsDlg::OnAddSnippet(wxCommandEvent& event)
{
    if(GetStringDb()->IsSnippetKey(m_textCtrlMenuEntry->GetValue())) {
        wxMessageBox(_("Menu entry is not unique!"));
        return;
    }
    GetStringDb()->SetSnippetString(m_textCtrlMenuEntry->GetValue(), m_textCtrlSnippet->GetValue());
    long index = m_listBox1->Append(m_textCtrlMenuEntry->GetValue());
    m_listBox1->SetSelection(index);
    m_modified = true;
}

void EditSnippetsDlg::OnAddSnippetUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlMenuEntry->GetValue().IsEmpty() && !m_textCtrlSnippet->GetValue().IsEmpty());
}

void EditSnippetsDlg::OnChangeSnippet(wxCommandEvent& event)
{
    wxString curListKey = m_listBox1->GetStringSelection();
    int index = m_listBox1->GetSelection();

    // check if list key is not equal new menu entry and if it is an used key
    if(curListKey.Cmp(m_textCtrlMenuEntry->GetValue()) != 0 &&
       GetStringDb()->IsSnippetKey(m_textCtrlMenuEntry->GetValue())) {
        ::wxMessageBox(_("Menu entry is not unique!"));
        return;
    }

    // if menu entry has changed, delete old entry in list
    if(curListKey.Cmp(m_textCtrlMenuEntry->GetValue()) != 0) {
        GetStringDb()->DeleteSnippetKey(curListKey);
    }

    GetStringDb()->SetSnippetString(m_textCtrlMenuEntry->GetValue(), m_textCtrlSnippet->GetValue());
    m_listBox1->SetString(index, m_textCtrlMenuEntry->GetValue());
    m_modified = true;
}

void EditSnippetsDlg::OnChangeSnippetUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlMenuEntry->GetValue().IsEmpty() && !m_textCtrlSnippet->GetValue().IsEmpty());
}

void EditSnippetsDlg::OnRemoveSnippet(wxCommandEvent& event)
{
    wxString key = m_listBox1->GetStringSelection();
    int index = m_listBox1->GetSelection();

    GetStringDb()->DeleteSnippetKey(key);
    m_listBox1->Delete(index);

    if(m_listBox1->GetCount()) {
        SelectItem(0);
    }
    m_modified = true;
}

void EditSnippetsDlg::OnRemoveSnippetUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listBox1->GetSelection() != wxNOT_FOUND);
}

void EditSnippetsDlg::Initialize()
{
    wxTextAttr attribs = m_textCtrlSnippet->GetDefaultStyle();
    wxArrayInt tabs = attribs.GetTabs();
    int tab = 70;
    for(int i = 1; i < 20; i++) {
        tabs.Add(tab * i);
    }

    attribs.SetTabs(tabs);
    m_textCtrlSnippet->SetDefaultStyle(attribs);

    wxArrayString keys;
    GetStringDb()->GetAllSnippetKeys(keys);
    m_listBox1->Append(keys);

    if(m_listBox1->IsEmpty() == false) {
        SelectItem(0);
    }
    m_htmlWinAbout->SetPage(wxString::FromUTF8(snipwizhtml_txt));
}

swStringDb* EditSnippetsDlg::GetStringDb() { return m_pPlugin->GetStringDb(); }

void EditSnippetsDlg::OnButtonKeyShortcut(wxCommandEvent& e)
{
    wxArrayString keys;
    GetStringDb()->GetAllSnippetKeys(keys);
    keys.Sort();

    int index = keys.Index(m_textCtrlMenuEntry->GetValue());
    if(index != wxNOT_FOUND) {
        int id = 20050 + index;

        MenuItemData mid;
        mid.resourceID << id;
        mid.parentMenu = _("SnipWiz");
        mid.action = m_textCtrlMenuEntry->GetValue();

        if(clKeyboardManager::Get()->PopupNewKeyboardShortcutDlg(this, mid) == wxID_OK) {

            if(clKeyboardManager::Get()->Exists(mid.accel)) {
                wxMessageBox(_("That accelerator already exists"), _("CodeLite"), wxOK | wxCENTRE, this);
                return;
            }

            clKeyboardManager::Get()->AddAccelerator(mid.resourceID, mid.parentMenu, mid.action, mid.accel);
            clKeyboardManager::Get()->Update();
            m_textCtrlAccelerator->ChangeValue(mid.accel.ToString());
        }
    }
}
