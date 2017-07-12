//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newclassdlg.cpp
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

#include "newclassdlg.h"
#include "open_resource_dialog.h"
#include "windowattrmanager.h"
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include "VirtualDirectorySelectorDlg.h"
#include "wx/xrc/xmlres.h"
#include <wx/tokenzr.h>
#include "newinheritancedlg.h"
#include "imanager.h"
#include "globals.h"
#include "wx/dir.h"
#include "workspace.h"
#include "editor_config.h"
#include "new_class_dlg_data.h"
#include <wx/filename.h>

NewClassDlg::NewClassDlg(wxWindow* parent, IManager* mgr)
    : NewClassBaseDlg(parent)
    , m_selectedItem(wxNOT_FOUND)
    , m_mgr(mgr)
{

    NewClassDlgData data;
    EditorConfigST::Get()->ReadObject(wxT("NewClassDlgData"), &data);

    m_checkBoxCopyable->SetValue(data.GetFlags() & NewClassDlgData::NonCopyable);
    m_checkBoxImplPureVirtual->SetValue(data.GetFlags() & NewClassDlgData::ImplAllPureVirtualFuncs);
    m_checkBoxImplVirtual->SetValue(data.GetFlags() & NewClassDlgData::ImplAllVirtualFuncs);
    m_checkBoxInline->SetValue(data.GetFlags() & NewClassDlgData::FileIniline);
    m_checkBoxHpp->SetValue(data.GetFlags() & NewClassDlgData::HppHeader);
    m_checkBoxSingleton->SetValue(data.GetFlags() & NewClassDlgData::Singleton);
    m_checkBoxVirtualDtor->SetValue(data.GetFlags() & NewClassDlgData::VirtualDtor);
    m_checkBoxPragmaOnce->SetValue(data.GetFlags() & NewClassDlgData::UsePragma);

    // set two columns to our list
    m_listCtrl1->InsertColumn(0, _("Name"));
    m_listCtrl1->InsertColumn(1, _("Access"));
    m_listCtrl1->InsertColumn(2, _("File"));

    wxString vdPath;
    TreeItemInfo item = mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory) {
        wxString path = VirtualDirectorySelectorDlg::DoGetPath(m_mgr->GetTree(TreeFileView), item.m_item, false);
        if(path.IsEmpty() == false) {
            m_textCtrlVD->ChangeValue(path);
        }
    }

    // set the class path to be the active project path
    wxString errMsg;
    if(m_mgr->GetWorkspace()) {
        wxString start_path;
        if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory) {
            m_basePath = item.m_fileName.GetPath(wxPATH_GET_VOLUME);

        } else {

            wxString projname = m_mgr->GetWorkspace()->GetActiveProjectName();
            ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projname, errMsg);
            if(proj) {
                m_basePath = proj->GetFileName().GetPath(wxPATH_GET_VOLUME);
            }
        }
    }

    DoUpdateGeneratedPath();

    GetSizer()->Layout();
    m_textClassName->SetFocus();

    GetSizer()->Fit(this);
    CentreOnParent();
}

NewClassDlg::~NewClassDlg() { DoSaveOptions(); }

void NewClassDlg::OnListItemActivated(wxListEvent& event)
{
    m_selectedItem = event.m_itemIndex;
    // open the inheritance dialog
    wxString parentName = GetColumnText(m_listCtrl1, m_selectedItem, 0);
    wxString access = GetColumnText(m_listCtrl1, m_selectedItem, 1);
    NewIneritanceDlg* dlg = new NewIneritanceDlg(NULL, m_mgr, parentName, access);
    if(dlg->ShowModal() == wxID_OK) {
        // now set the text to this column
        SetColumnText(m_listCtrl1, m_selectedItem, 0, dlg->GetParentName());
        SetColumnText(m_listCtrl1, m_selectedItem, 1, dlg->GetAccess());

        SetColumnText(m_listCtrl1, m_selectedItem, 2, dlg->GetFileName());

        m_listCtrl1->Refresh();
    }
    dlg->Destroy();
}

void NewClassDlg::OnListItemSelected(wxListEvent& event) { m_selectedItem = event.m_itemIndex; }

void NewClassDlg::OnButtonAdd(wxCommandEvent& event)
{
    NewIneritanceDlg* dlg = new NewIneritanceDlg(this, m_mgr);
    if(dlg->ShowModal() == wxID_OK) {
        // add new parent to our class
        // now set the text to this column
        long item = AppendListCtrlRow(m_listCtrl1);

        SetColumnText(m_listCtrl1, item, 0, dlg->GetParentName());
        SetColumnText(m_listCtrl1, item, 1, dlg->GetAccess());

        SetColumnText(m_listCtrl1, item, 2, dlg->GetFileName());

        m_listCtrl1->Refresh();
    }
    dlg->Destroy();
}

void NewClassDlg::OnListItemDeSelected(wxListEvent& e)
{
    wxUnusedVar(e);
    m_selectedItem = wxNOT_FOUND;
}

void NewClassDlg::OnButtonDelete(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_listCtrl1->DeleteItem(m_selectedItem);
    m_selectedItem = wxNOT_FOUND;
}

void NewClassDlg::OnButtonDeleteUI(wxUpdateUIEvent& event) { event.Enable(m_selectedItem != wxNOT_FOUND); }

void NewClassDlg::GetInheritance(std::vector<ClassParentInfo>& inheritVec)
{
    long item = -1;
    for(;;) {
        item = m_listCtrl1->GetNextItem(item);
        if(item == -1) break;

        ClassParentInfo info;
        info.name = GetColumnText(m_listCtrl1, item, 0);
        info.access = GetColumnText(m_listCtrl1, item, 1);
        info.fileName = GetColumnText(m_listCtrl1, item, 2);

        inheritVec.push_back(info);
    }
}

void NewClassDlg::OnButtonOK(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(!ValidateInput()) {
        return;
    }
    DoSaveOptions();
    EndModal(wxID_OK);
}

/**
 * \brief
 */
bool NewClassDlg::ValidateInput()
{
    // validate the class name
    if(!IsValidCppIndetifier(m_textClassName->GetValue())) {
        wxString msg;
        msg << wxT("'") << m_textClassName->GetValue() << _("' is not a valid C++ qualifier");
        wxMessageBox(msg, _("CodeLite"), wxOK | wxICON_WARNING);
        return false;
    }

    // validate the namespace
    if(!m_textCtrlNamespace->GetValue().IsEmpty()) {
        wxArrayString namespacesList;
        this->GetNamespacesList(namespacesList);
        // validate each namespace
        for(unsigned int i = 0; i < namespacesList.Count(); i++) {
            if(!IsValidCppIndetifier(namespacesList[i])) {
                wxString msg;
                msg << wxT("'") << namespacesList[i] << _("' is not a valid C++ qualifier");
                wxMessageBox(msg, _("CodeLite"), wxOK | wxICON_WARNING);
                return false;
            }
        }
    }

    // validate the path of the class
    wxString path(m_textCtrlGenFilePath->GetValue());
    if(!wxDir::Exists(path)) {
        wxString msg;
        msg << wxT("'") << path << _("': directory does not exist");
        wxMessageBox(msg, _("CodeLite"), wxOK | wxICON_WARNING);
        return false;
    }

    if(GetClassFile().IsEmpty()) {
        wxMessageBox(_("Empty file name"), _("CodeLite"), wxOK | wxICON_WARNING);
        return false;
    }

    wxString cpp_file;
    cpp_file << GetClassPath() << wxFileName::GetPathSeparator() << GetClassFile() << wxT(".cpp");
    if(wxFileName::FileExists(cpp_file)) {
        if(wxMessageBox(
               wxString::Format(_("A file with this name: '%s' already exists, continue anyway?"), cpp_file.GetData()),
               _("CodeLite"),
               wxYES_NO | wxICON_WARNING) == wxNO) {
            return false;
        }
    }
    wxString h_file;
    h_file << GetClassPath() << wxFileName::GetPathSeparator() << GetClassFile() << wxT(".h");
    if(wxFileName::FileExists(h_file)) {
        if(wxMessageBox(
               wxString::Format(_("A file with this name: '%s' already exists, continue anyway?"), h_file.GetData()),
               _("CodeLite"),
               wxYES_NO | wxICON_WARNING) == wxNO) {
            return false;
        }
    }

    if(GetVirtualDirectoryPath().IsEmpty()) {
        wxMessageBox(_("Please select a virtual directory"), _("CodeLite"), wxOK | wxICON_WARNING);
        return false;
    }
    return true;
}

void NewClassDlg::GetNewClassInfo(NewClassInfo& info)
{
    info.name = this->GetClassName();
    this->GetNamespacesList(info.namespacesList);
    this->GetInheritance(info.parents);
    if(this->IsInline()) {
        info.isInline = true;
        info.isSingleton = false;
    } else {
        info.isInline = false;
        info.isSingleton = this->IsSingleton();
    }
    info.hppHeader = this->HppHeader();
    info.path = this->GetClassPath().Trim().Trim(false);
    info.isAssingable = this->IsCopyableClass();
    info.fileName = wxFileName(GetClassFile()).GetName(); // Ommit any suffix the user might have typed in
    info.isVirtualDtor = m_checkBoxVirtualDtor->IsChecked();
    info.implAllPureVirtual = m_checkBoxImplPureVirtual->IsChecked();
    info.implAllVirtual = m_checkBoxImplVirtual->IsChecked();
    info.usePragmaOnce = m_checkBoxPragmaOnce->IsChecked();
    info.virtualDirectory = m_textCtrlVD->GetValue().Trim().Trim(false);
    info.blockGuard = m_textCtrlBlockGuard->GetValue().Trim().Trim(false);
}

wxString NewClassDlg::GetClassFile() { return m_textCtrlFileName->GetValue(); }

void NewClassDlg::OnTextEnter(wxCommandEvent& e) { m_textCtrlFileName->ChangeValue(m_textClassName->GetValue()); }

void NewClassDlg::OnCheckImpleAllVirtualFunctions(wxCommandEvent& e)
{
    if(e.IsChecked()) {
        m_checkBoxImplPureVirtual->SetValue(true);
    }
}

wxString NewClassDlg::GetClassPath()
{
    if(m_textCtrlGenFilePath->GetValue().Trim().IsEmpty()) {
        return wxT(".");
    } else {
        return m_textCtrlGenFilePath->GetValue();
    }
}

void NewClassDlg::OnBrowseFolder(wxCommandEvent& e)
{
    wxUnusedVar(e);
    wxString initPath;
    if(wxFileName::DirExists(m_textCtrlGenFilePath->GetValue())) {
        initPath = m_textCtrlGenFilePath->GetValue();
    }
    wxString new_path =
        wxDirSelector(_("Select Generated Files Path:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {
        m_textCtrlGenFilePath->ChangeValue(new_path);
    }
}

void NewClassDlg::OnBrowseVD(wxCommandEvent& e)
{
    wxUnusedVar(e);
    VirtualDirectorySelectorDlg dlg(this, m_mgr->GetWorkspace(), m_textCtrlVD->GetValue());
    if(dlg.ShowModal() == wxID_OK) {
        m_textCtrlVD->ChangeValue(dlg.GetVirtualDirectoryPath());
        DoUpdateGeneratedPath();
    }
}

void NewClassDlg::OnBrowseNamespace(wxCommandEvent& e)
{
    wxUnusedVar(e);

    wxArrayString kinds;
    kinds.Add(wxT("namespace"));

    OpenResourceDialog dlg(this, m_mgr, "");
    if(dlg.ShowModal() == wxID_OK) {
        std::vector<OpenResourceDialogItemData*> selections = dlg.GetSelections();
        if(!selections.empty()) {
            OpenResourceDialogItemData* item = selections.at(0);
            wxString nameSpace;
            if(item->m_scope.IsEmpty() == false && item->m_scope != wxT("<global>")) {
                nameSpace << item->m_scope << wxT("::");
            }
            nameSpace << item->m_name;
            m_textCtrlNamespace->ChangeValue(nameSpace);

        }
    }
}

void NewClassDlg::GetNamespacesList(wxArrayString& namespacesArray)
{
    wxString textNamespaces = this->GetClassNamespace();
    textNamespaces.Trim();

    if(textNamespaces.IsEmpty()) return;

    int prevPos = 0;
    size_t pos = textNamespaces.find(wxT("::"), prevPos);

    while(pos != wxString::npos) {
        wxString token = textNamespaces.Mid(prevPos, pos - prevPos);

        namespacesArray.Add(token);

        prevPos = pos + 2;
        pos = textNamespaces.find(wxT("::"), prevPos);
    }

    wxString lastToken = textNamespaces.Mid(prevPos);
    namespacesArray.Add(lastToken);
}

void NewClassDlg::OnCheckInline(wxCommandEvent& e)
{
    // Inline implementation conflict with singleton implementation
    // so disable the relative checkbox
    if(e.IsChecked()) {
        if(m_checkBoxSingleton->IsEnabled()) m_checkBoxSingleton->Enable(false);
    } else {
        if(!m_checkBoxSingleton->IsEnabled()) m_checkBoxSingleton->Enable(true);
    }
}

wxString NewClassDlg::doSpliteByCaptilization(const wxString& str)
{
    if(str.IsEmpty()) return wxT("");

    wxString output;
    bool lastWasLower(true);

    for(int i = str.length() - 1; i >= 0; --i) {

        int cur = (int)str[i];
        if(!isalpha(cur)) {
            output.Prepend((wxChar)cur);
            continue;
        }

        if(isupper(cur) && lastWasLower) {
            output.Prepend((wxChar)cur);
            output.Prepend(wxT('_'));

        } else {
            output.Prepend((wxChar)cur);
        }

        lastWasLower = islower(cur);
    }

    // replace any double underscores with single one
    while(output.Replace(wxT("__"), wxT("_"))) {
    }

    // remove any underscore from the start of the word
    if(output.StartsWith(wxT("_"))) {
        output.Remove(0, 1);
    }
    return output;
}

void NewClassDlg::DoUpdateGeneratedPath()
{
    wxString vdPath = m_textCtrlVD->GetValue();

    wxString project, vd, errMsg;
    project = vdPath.BeforeFirst(wxT(':'));
    vd = vdPath.AfterFirst(wxT(':'));

    ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project, errMsg);
    if(proj) {
        m_textCtrlGenFilePath->ChangeValue(proj->GetBestPathForVD(vd));
    }
}

void NewClassDlg::OnOkUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(!(GetClassFile().IsEmpty() || GetVirtualDirectoryPath().IsEmpty()));
}
void NewClassDlg::OnBlockGuardUI(wxUpdateUIEvent& event) { event.Enable(!m_checkBoxPragmaOnce->IsChecked()); }

void NewClassDlg::DoSaveOptions()
{
    // Save the check boxes ticked
    size_t flags(0);

    if(m_checkBoxCopyable->IsChecked()) flags |= NewClassDlgData::NonCopyable;
    if(m_checkBoxImplPureVirtual->IsChecked()) flags |= NewClassDlgData::ImplAllPureVirtualFuncs;
    if(m_checkBoxImplVirtual->IsChecked()) flags |= NewClassDlgData::ImplAllVirtualFuncs;
    if(m_checkBoxInline->IsChecked()) flags |= NewClassDlgData::FileIniline;
    if(m_checkBoxHpp->IsChecked()) flags |= NewClassDlgData::HppHeader;
    if(m_checkBoxSingleton->IsChecked()) flags |= NewClassDlgData::Singleton;
    if(m_checkBoxVirtualDtor->IsChecked()) flags |= NewClassDlgData::VirtualDtor;
    if(m_checkBoxPragmaOnce->IsChecked()) flags |= NewClassDlgData::UsePragma;

    NewClassDlgData data;
    data.SetFlags(flags);
    EditorConfigST::Get()->WriteObject(wxT("NewClassDlgData"), &data);
}
