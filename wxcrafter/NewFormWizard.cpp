#include "NewFormWizard.h"
#include "VirtualDirectorySelectorDlg.h"
#include "allocator_mgr.h"
#include "clFileSystemWorkspace.hpp"
#include "wxc_project_metadata.h"
#include "wxc_settings.h"
#include <macros.h>
#include <project.h>
#include <workspace.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

static wxString GetDisplayName(const wxFileName& fn)
{
    wxString displayName;
    displayName << fn.GetDirs().Last() << wxFileName::GetPathSeparator() << fn.GetFullName();
    return displayName;
}

NewFormWizard::NewFormWizard(wxWindow* parent, IManager* mgr, int type)
    : NewFormWizardBaseClass(parent)
    , m_mgr(mgr)
{
    wxString stringSelection;
    switch(type) {
    case ID_WXDIALOG:
        stringSelection = wxT("wxDialog");
        break;
    case ID_WXFRAME:
        stringSelection = wxT("wxFrame");
        break;
    case ID_WXWIZARD:
        stringSelection = wxT("wxWizard");
        break;
    case ID_WXPANEL_TOPLEVEL:
        stringSelection = wxT("wxPanel");
        break;
    case ID_WXIMAGELIST:
        stringSelection = wxT("wxImageList");
        break;
    case ID_WXPOPUPWINDOW:
        stringSelection = wxT("wxPopupWindow");
        break;
    case ID_WXAUITOOLBARTOPLEVEL:
        stringSelection = "wxAuiToolBar";
        break;
    }

    if(stringSelection.IsEmpty() == false)
        m_choiceFormType->SetStringSelection(stringSelection);

    if(m_mgr) {
        TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
        if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory) {
            wxString path = VirtualDirectorySelectorDlg::DoGetPath(m_mgr->GetWorkspaceTree(), item.m_item, false);
            if(path.IsEmpty() == false) {
                m_textCtrlVirtualFolder->ChangeValue(path);
                m_textCtrlVirtualFolder->SetEditable(false);
            }
        }
    }

    // Populate the *wxcp files
    std::set<wxString> uniqueFiles;
    std::vector<std::pair<wxString, wxString>> wxcp_files;

    if(clCxxWorkspaceST::Get()->IsOpen()) {
        wxArrayString allFiles;
        clCxxWorkspaceST::Get()->GetWorkspaceFiles(allFiles);
        for(size_t i = 0; i < allFiles.GetCount(); ++i) {
            const wxString& filename = allFiles.Item(i);
            wxFileName fn(filename);
            if(fn.GetExt().Lower() == "wxcp") {
                if(uniqueFiles.count(filename) == 0) {
                    uniqueFiles.insert(filename);

                    // we keep {display name:fullpath} pairs
                    wxcp_files.push_back({ GetDisplayName(fn), fn.GetFullPath() });
                }
            }
        }

        for(const auto& p : wxcp_files) {
            m_choiceWxcp->Append(p.first, new wxStringClientData(p.second));
        }
    }

    if(wxcProjectMetadata::Get().IsLoaded()) {
        wxFileName fn(wxcProjectMetadata::Get().GetProjectFile());
        m_choiceWxcp->Append(GetDisplayName(fn), new wxStringClientData(fn.GetFullPath()));
    }

    if(!m_choiceWxcp->IsEmpty()) {
        m_choiceWxcp->SetSelection(0);
    }

    wxFileName fnProject(wxcProjectMetadata::Get().GetProjectFile());
    int where = m_choiceWxcp->FindString(GetDisplayName(fnProject));
    if(where != wxNOT_FOUND) {
        m_choiceWxcp->SetSelection(where);
    }
}

NewFormWizard::~NewFormWizard() {}

void NewFormWizard::OnFormTypeSelected(wxCommandEvent& event) { event.Skip(); }

void NewFormWizard::OnSelectVirtualFolder(wxCommandEvent& event)
{
    wxString suggestedName = m_textCtrlVirtualFolder->GetValue();
    if(suggestedName.empty() && clCxxWorkspaceST::Get()) {
        suggestedName = clCxxWorkspaceST::Get()->GetActiveProjectName();
    }
    VirtualDirectorySelectorDlg selector(this, clCxxWorkspaceST::Get(), suggestedName);
    if(selector.ShowModal() == wxID_OK) {
        m_textCtrlVirtualFolder->SetEditable(true);
        m_textCtrlVirtualFolder->ChangeValue(selector.GetVirtualDirectoryPath());
        m_textCtrlVirtualFolder->SetEditable(false);
    }
}

void NewFormWizard::OnFinishClicked(wxWizardEvent& event) { event.Skip(); }

int NewFormWizard::GetFormType() const
{
    if(IsDialog())
        return ID_WXDIALOG;
    else if(IsFrame())
        return ID_WXFRAME;
    else if(IsWizard())
        return ID_WXWIZARD;
    else if(IsPanel())
        return ID_WXPANEL_TOPLEVEL;
    else if(IsImageList())
        return ID_WXIMAGELIST;
    else if(IsAuiToolBar())
        return ID_WXAUITOOLBARTOPLEVEL;
    else if(IsPopupWindow())
        return ID_WXPOPUPWINDOW;
    return wxNOT_FOUND;
}

wxString NewFormWizard::GetVirtualFolder() const { return m_textCtrlVirtualFolder->GetValue(); }

wxString NewFormWizard::GetClassName() const { return m_textCtrlClassName->GetValue(); }

wxString NewFormWizard::GetGeneratedFileBaseName() const { return m_textCtrFileName->GetValue(); }

void NewFormWizard::OnWizardPageChanging(wxWizardEvent& event)
{
    if(event.GetDirection() && m_wizardPageGeneratedCode == event.GetPage()) {

#if STANDALONE_BUILD
        if(m_choiceWxcp->IsEmpty()) {
            ::wxMessageBox(wxString() << _("You must create a project before you can add new forms"), "wxCrafter",
                           wxOK | wxCENTER | wxICON_WARNING);
            event.Veto();
            return;
        }
#endif

        if(m_textCtrFileName->IsEmpty() &&
           (GetFormType() != ID_WXIMAGELIST && GetFormType() != ID_WXAUITOOLBARTOPLEVEL)) {
            ::wxMessageBox(_("Please enter a file name"), wxT("wxCrafter"), wxICON_WARNING | wxCENTER | wxOK);
            event.Veto();
            return;
        }

        if(m_textCtrlClassName->IsEmpty()) {
            ::wxMessageBox(_("Please enter a class name"), wxT("wxCrafter"), wxICON_WARNING | wxCENTER | wxOK);
            event.Veto();
            return;
        }

#if !STANDALONE_BUILD
        if(!clFileSystemWorkspace::Get().IsOpen() && m_textCtrlVirtualFolder->IsEmpty()) {
            ::wxMessageBox(_("Please select a virtual folder for the generated code"), wxT("wxCrafter"),
                           wxICON_WARNING | wxCENTER | wxOK);
            event.Veto();
            return;
        }
#endif
    }
    event.Skip();
}

void NewFormWizard::OnMakeSkeletonAppUI(wxUpdateUIEvent& event)
{
    int formType = GetFormType();
    event.Enable(formType != ID_WXPANEL_TOPLEVEL);
}

wxString NewFormWizard::GetTitle() const { return m_textCtrlTitle->GetValue(); }

void NewFormWizard::OnNewWxcpProject(wxCommandEvent& event)
{
    wxString msg;
    msg << _("Enter the new wxCrafter file a name (full path):");

    wxFileName defaultValue(::wxGetCwd(), "my_wxcp_file.wxcp");

    if(m_mgr) {
        ProjectPtr project = m_mgr->GetSelectedProject();
        if(project) {
            defaultValue.SetPath(project->GetFileName().GetPath());
        } else {
            defaultValue.SetPath(::wxGetCwd());
        }
        defaultValue.SetFullName(wxT("my_wxcp_file.wxcp"));
    }

    wxString newfile = ::wxGetTextFromUser(msg, wxT("wxCrafter"), defaultValue.GetFullPath());
    if(newfile.IsEmpty())
        return;

    wxFileName fn(newfile);
    fn.SetExt(wxT("wxcp"));

    wxString displayName = GetDisplayName(fn);
    int where = m_choiceWxcp->FindString(displayName);
    if(where == wxNOT_FOUND) {
        int newpos = m_choiceWxcp->Append(displayName, new wxStringClientData(fn.GetFullPath()));
        m_choiceWxcp->SetSelection(newpos);

    } else {
        // already exists
        m_choiceWxcp->SetSelection(where);
    }
}

wxString NewFormWizard::GetWxcpFile() const
{
    int selection = m_choiceWxcp->GetSelection();
    if(selection == wxNOT_FOUND) {
        return "";
    }

    wxStringClientData* cd = reinterpret_cast<wxStringClientData*>(m_choiceWxcp->GetClientObject(selection));
    wxFileName fn(cd->GetData());
    return fn.GetFullPath();
}

void NewFormWizard::OnTitleUI(wxUpdateUIEvent& event)
{
    event.Enable(!IsImageList() && !IsPanel() && !IsPopupWindow() && !IsAuiToolBar());
}

void NewFormWizard::OnFilenameUI(wxUpdateUIEvent& event) { event.Enable(!IsImageList()); }

void NewFormWizard::OnInheritedClassNameUI(wxUpdateUIEvent& event) { event.Enable(!IsImageList()); }

wxString NewFormWizard::GetInheritedClassName() const
{
    return m_textCtrlInheritedClassName->GetValue().Trim().Trim(false);
}

void NewFormWizard::OnStandloneAppUI(wxUpdateUIEvent& event)
{
#if STANDALONE_BUILD
    event.Enable(false);
#else
    event.Enable(true);
#endif
}

void NewFormWizard::OnInheritedNameFocus(wxFocusEvent& event)
{
    event.Skip();

    if(!m_textCtrlInheritedClassName->IsEmpty() || m_textCtrlClassName->IsEmpty()) {
        // There's nothing to copy, or we already have an entry
        return;
    }

    wxString suggestion = m_textCtrlClassName->GetValue();
    if(suggestion.Replace("Base", "") || suggestion.Replace("base", "")) {
        m_textCtrlInheritedClassName->ChangeValue(suggestion);
    }
}

void NewFormWizard::OnFilenameFocus(wxFocusEvent& event)
{
    event.Skip();

    if(!m_textCtrFileName->IsEmpty() || m_textCtrlInheritedClassName->IsEmpty()) {
        // There's nothing to copy, or we already have an entry
        return;
    }

    // Make the default file name same as the class name
    wxString suggestion = m_textCtrlInheritedClassName->GetValue();
    m_textCtrFileName->ChangeValue(suggestion);
}

bool NewFormWizard::IsAuiToolBar() const { return m_choiceFormType->GetStringSelection() == "wxAuiToolBar"; }

bool NewFormWizard::IsDialog() const { return m_choiceFormType->GetStringSelection() == "wxDialog"; }

bool NewFormWizard::IsFrame() const { return m_choiceFormType->GetStringSelection() == "wxFrame"; }

bool NewFormWizard::IsImageList() const { return m_choiceFormType->GetStringSelection() == "wxImageList"; }

bool NewFormWizard::IsPanel() const { return m_choiceFormType->GetStringSelection() == "wxPanel"; }

bool NewFormWizard::IsPopupWindow() const { return m_choiceFormType->GetStringSelection() == "wxPopupWindow"; }

bool NewFormWizard::IsWizard() const { return m_choiceFormType->GetStringSelection() == "wxWizard"; }

void NewFormWizard::OnSelectVDUI(wxUpdateUIEvent& event) { event.Enable(!clFileSystemWorkspace::Get().IsOpen()); }

void NewFormWizard::OnBrowseWxcpFile(wxCommandEvent& event)
{
    wxString path = wxFileSelector(_("Select wxCrafter file"), wxEmptyString, wxEmptyString, "*.wxcp");
    if(path.empty()) {
        return;
    }

    wxFileName fn(path);
    wxString displayName = GetDisplayName(fn);
    int where = m_choiceWxcp->FindString(displayName);
    if(where == wxNOT_FOUND) {
        where = m_choiceWxcp->Append(displayName, new wxStringClientData(path));
    }
    m_choiceWxcp->SetSelection(where);
}
