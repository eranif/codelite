#include "import_dlg.h"
#include "VirtualDirectorySelectorDlg.h"
#include "wxgui_helpers.h"
#include <windowattrmanager.h>
#include <workspace.h>
#include <wx/app.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>

ImportDlg::ImportDlg(IPD_ProjectType type, wxWindow* parent, const wxString& sourceFile)
    : ImportDlgBaseClass(parent)
    , m_Type(type)
    , m_modified(false)
{
    if(m_Type == IPD_FB) {
        SetLabel(_("Choose a wxFormBuilder project to import"));
    } else if(m_Type == IPD_Smith) {
        SetLabel(_("Choose a wxSmith project to import"));
    }

    wxFileName fn(sourceFile);
    fn.SetExt("wxcp");
    m_filepathText->ChangeValue(sourceFile);
    m_textName->ChangeValue(fn.GetFullPath());

#if STANDALONE_BUILD
    m_checkBoxAddToProject->SetValue(false);
#endif
    SetName("ImportDlg");
    WindowAttrManager::Load(this);
}

ImportDlg::~ImportDlg() {}

void ImportDlg::OnBrowse(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString title, ext;
    if(m_Type == IPD_FB) {
        ext << (wxT("wxFB Project (*.fbp)|*.fbp|Any File (")) << wxFileSelectorDefaultWildcardStr << wxT(")|")
            << wxFileSelectorDefaultWildcardStr;
        title = _("Import a wxFormBuilder project");
    }
    if(m_Type == IPD_Smith) {
        ext << (wxT("wxSmith Project (*.wxs)|*.wxs|Any File (")) << wxFileSelectorDefaultWildcardStr << wxT(")|")
            << wxFileSelectorDefaultWildcardStr;
        title = _("Import a wxSmith project");
    }
    if(m_Type == IPD_XRC) {
        ext << (wxT("XRC File (*.xrc)|*.xrc|Any File (")) << wxFileSelectorDefaultWildcardStr << wxT(")|")
            << wxFileSelectorDefaultWildcardStr;
        title = _("Import an XRC file");
    }
    wxString path = wxFileSelector(title, wxEmptyString, wxEmptyString, wxEmptyString, ext, wxFD_OPEN, this);
    if(path.empty()) { return; }
    m_filepathText->ChangeValue(path);

    // Use that filepath as the suggested destination, but using .wxcp as the ext
    wxFileName fn(path);
    fn.SetExt("wxcp");
    m_textName->ChangeValue(fn.GetFullPath());
}

void ImportDlg::OnOKButtonUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_filepathText->IsEmpty() && !m_textName->IsEmpty());
}

void ImportDlg::OnBrowseForOutputFolder(wxCommandEvent& event)
{
    wxString curtext = m_textName->GetValue();
    wxFileName fn(curtext);
    wxString dir = wxDirSelector();
    if(dir.IsEmpty()) return;

    wxFileName newpath(dir, fn.GetFullName());
    m_textName->ChangeValue(newpath.GetFullPath());
}

void ImportDlg::OnBrowseForVirtualFolder(wxCommandEvent& event)
{
    VirtualDirectorySelectorDlg vds(wxCrafter::TopFrame(), clCxxWorkspaceST::Get());
    if(vds.ShowModal() == wxID_OK) {
        wxString vd = vds.GetVirtualDirectoryPath();
        m_textCtrl1VirtualFolder->ChangeValue(vd);
    }
}

void ImportDlg::OnAddFileToProjectUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxAddToProject->IsChecked()); }

ImportDlg::ImportFileData ImportDlg::GetData() const
{
    ImportFileData d;
    d.addToProject = m_checkBoxAddToProject->IsChecked();
    d.loadWhenDone = m_checkBoxLoadWhenDone->IsChecked();
    d.wxcpFilename = m_textName->GetValue();
    d.virtualFolder = m_textCtrl1VirtualFolder->GetValue();
    return d;
}
void ImportDlg::OnAddToProjectUI(wxUpdateUIEvent& event)
{
#if STANDALONE_BUILD
    event.Enable(false);
#endif
}

void ImportDlg::OnFileImportTextUpdated(wxCommandEvent& event)
{
    if(!m_modified) {
        wxFileName fnWXCPFile(m_filepathText->GetValue());
        fnWXCPFile.SetExt("wxcp");
        m_textName->ChangeValue(fnWXCPFile.GetFullPath());
    }
}

void ImportDlg::OnDestFilePathUpdated(wxCommandEvent& event)
{
    event.Skip();
    m_modified = true;
}
