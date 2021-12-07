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
#include "VirtualDirectorySelectorDlg.h"
#include "clFileSystemWorkspace.hpp"
#include "clFileSystemWorkspaceView.hpp"
#include "editor_config.h"
#include "globals.h"
#include "imanager.h"
#include "new_class_dlg_data.h"
#include "open_resource_dialog.h"
#include "windowattrmanager.h"
#include "workspace.h"
#include "wx/dir.h"
#include "wx/xrc/xmlres.h"
#include <wx/dirdlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

NewClassDlg::NewClassDlg(wxWindow* parent, IManager* mgr)
    : NewClassBaseDlg(parent)
    , m_selectedItem(wxNOT_FOUND)
    , m_mgr(mgr)
{
    EditorConfigST::Get()->ReadObject(wxT("NewClassDlgData"), &m_options);

    m_checkBoxNonCopyable->SetValue(m_options.GetFlags() & NewClassDlgData::NonCopyable);
    m_checkBoxInline->SetValue(m_options.GetFlags() & NewClassDlgData::FileIniline);
    m_checkBoxHpp->SetValue(m_options.GetFlags() & NewClassDlgData::HppHeader);
    m_checkBoxSingleton->SetValue(m_options.GetFlags() & NewClassDlgData::Singleton);
    m_checkBoxVirtualDtor->SetValue(m_options.GetFlags() & NewClassDlgData::VirtualDtor);
    m_checkBoxPragmaOnce->SetValue(m_options.GetFlags() & NewClassDlgData::UsePragma);
    m_checkBoxLowercaseFileName->SetValue(m_options.GetFlags() & NewClassDlgData::UseLowerCase);
    m_checkBoxNonMovable->SetValue(m_options.GetFlags() & NewClassDlgData::NonMovable);
    m_checkBoxNonInheritable->SetValue(m_options.GetFlags() & NewClassDlgData::NonInheritable);

    wxString vdPath;
    TreeItemInfo item = mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory) {
        wxString path = VirtualDirectorySelectorDlg::DoGetPath(m_mgr->GetWorkspaceTree(), item.m_item, false);
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
    DoUpdateCheckBoxes();

    GetSizer()->Layout();
    m_textClassName->SetFocus();

    // Check for file system workspace case
    if(clFileSystemWorkspace::Get().IsOpen()) {
        wxArrayString folders, files;
        clFileSystemWorkspace::Get().GetView()->GetSelections(folders, files);
        if(folders.size() == 1) {
            m_textCtrlGenFilePath->ChangeValue(folders.Item(0));
        }
        m_textCtrlVD->Disable();
        m_buttonSelectVD->Disable();
        m_staticTextVD->Disable();
    }
    ::clSetSmallDialogBestSizeAndPosition(this);
}

NewClassDlg::~NewClassDlg() { DoSaveOptions(); }

void NewClassDlg::GetInheritance(ClassParentInfo& parent) const
{
    parent.access = "public";
    parent.fileName = m_parentClass;
    parent.name = m_textCtrlParentClass->GetValue();
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
               _("CodeLite"), wxYES_NO | wxICON_WARNING) == wxNO) {
            return false;
        }
    }
    wxString h_file;
    h_file << GetClassPath() << wxFileName::GetPathSeparator() << GetClassFile() << wxT(".h");
    if(wxFileName::FileExists(h_file)) {
        if(wxMessageBox(
               wxString::Format(_("A file with this name: '%s' already exists, continue anyway?"), h_file.GetData()),
               _("CodeLite"), wxYES_NO | wxICON_WARNING) == wxNO) {
            return false;
        }
    }

    if(!clFileSystemWorkspace::Get().IsOpen() && GetVirtualDirectoryPath().IsEmpty()) {
        wxMessageBox(_("Please select a virtual directory"), _("CodeLite"), wxOK | wxICON_WARNING);
        return false;
    }
    return true;
}

void NewClassDlg::GetNewClassInfo(NewClassInfo& info) const
{
    info.name = this->GetClassName();
    this->GetNamespacesList(info.namespacesList);
    this->GetInheritance(info.parents);
    info.isInline = this->IsInline();
    info.isSingleton = this->IsSingleton();
    info.hppHeader = this->HppHeader();
    info.path = this->GetClassPath().Trim().Trim(false);
    info.isAssignable = this->IsCopyableClass();
    info.isMovable = this->IsMovableClass();
    info.isInheritable = this->IsInheritable();
    info.fileName = wxFileName(GetClassFile()).GetName(); // Ommit any suffix the user might have typed in
    info.isVirtualDtor = m_checkBoxVirtualDtor->IsChecked();
    info.usePragmaOnce = m_checkBoxPragmaOnce->IsChecked();
    info.virtualDirectory = m_textCtrlVD->GetValue().Trim().Trim(false);
    info.blockGuard = m_textCtrlBlockGuard->GetValue().Trim().Trim(false);
}

wxString NewClassDlg::GetClassFile() const { return m_textCtrlFileName->GetValue(); }

void NewClassDlg::OnTextEnter(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_textCtrlFileName->ChangeValue(CreateFileName());
}

void NewClassDlg::OnCheckSingleton(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoUpdateCheckBoxes();
}

void NewClassDlg::OnCheckImpleAllVirtualFunctions(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoUpdateCheckBoxes();
}

wxString NewClassDlg::GetClassPath() const
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

void NewClassDlg::GetNamespacesList(wxArrayString& namespacesArray) const
{
    wxString textNamespaces = this->GetClassNamespace();
    textNamespaces.Trim();

    if(textNamespaces.IsEmpty())
        return;

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

wxString NewClassDlg::doSpliteByCaptilization(const wxString& str) const
{
    if(str.IsEmpty())
        return wxT("");

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
    while(output.Replace(wxT("__"), wxT("_"))) {}

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

void NewClassDlg::DoUpdateCheckBoxes()
{
    bool singleton = m_checkBoxSingleton->IsChecked();

    if(singleton) {
        // Singleton class must not be copyable or movable
        m_checkBoxNonCopyable->SetValue(true);
        m_checkBoxNonMovable->SetValue(true);
        // A static pointer variable needs to be defined in the source file
        m_checkBoxInline->SetValue(false);
    }

    m_checkBoxNonCopyable->Enable(!singleton);
    m_checkBoxNonMovable->Enable(!singleton);
    m_checkBoxInline->Enable(!singleton);
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

    if(m_checkBoxNonCopyable->IsChecked())
        flags |= NewClassDlgData::NonCopyable;
    if(m_checkBoxInline->IsChecked())
        flags |= NewClassDlgData::FileIniline;
    if(m_checkBoxHpp->IsChecked())
        flags |= NewClassDlgData::HppHeader;
    if(m_checkBoxSingleton->IsChecked())
        flags |= NewClassDlgData::Singleton;
    if(m_checkBoxVirtualDtor->IsChecked())
        flags |= NewClassDlgData::VirtualDtor;
    if(m_checkBoxPragmaOnce->IsChecked())
        flags |= NewClassDlgData::UsePragma;
    if(m_checkBoxLowercaseFileName->IsChecked())
        flags |= NewClassDlgData::UseLowerCase;
    if(m_checkBoxNonMovable->IsChecked())
        flags |= NewClassDlgData::NonMovable;
    if(m_checkBoxNonInheritable->IsChecked())
        flags |= NewClassDlgData::NonInheritable;

    m_options.SetFlags(flags);
    EditorConfigST::Get()->WriteObject(wxT("NewClassDlgData"), &m_options);
}

void NewClassDlg::OnUseLowerCaseFileName(wxCommandEvent& event)
{
    m_options.EnableFlag(NewClassDlgData::UseLowerCase, event.IsChecked());
    m_textCtrlFileName->ChangeValue(CreateFileName());
}

wxString NewClassDlg::CreateFileName() const
{
    if(m_options.HasFlag(NewClassDlgData::UseLowerCase)) {
        return m_textClassName->GetValue().Lower();
    }
    return m_textClassName->GetValue();
}

void NewClassDlg::OnBrowseParentClass(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxArrayString kinds;
    kinds.Add(wxT("class"));
    kinds.Add(wxT("struct"));

    OpenResourceDialog dlg(this, m_mgr, "");
    if(dlg.ShowModal() == wxID_OK) {
        std::vector<OpenResourceDialogItemData*> selections = dlg.GetSelections();
        if(!selections.empty()) {
            OpenResourceDialogItemData* item = selections[0];
            wxString fullpathName;
            if(item->m_scope.empty()) {
                fullpathName << item->m_name;
            } else {
                fullpathName << item->m_scope << "::" << item->m_name;
            }
            m_textCtrlParentClass->ChangeValue(fullpathName);
            m_parentClass = wxFileName(item->m_file).GetFullName();
        }
    }
}
