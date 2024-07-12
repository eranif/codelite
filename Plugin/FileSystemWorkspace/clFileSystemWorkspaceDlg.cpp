#include "clFileSystemWorkspaceDlg.h"

#include "BuildTargetDlg.h"
#include "ColoursAndFontsManager.h"
#include "FSConfigPage.h"
#include "clFSWNewConfigDlg.h"
#include "clFileSystemWorkspace.hpp"
#include "globals.h"
#include "macros.h"

#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/wupdlock.h>

clFileSystemWorkspaceDlg::clFileSystemWorkspaceDlg(wxWindow* parent, clFileSystemWorkspaceSettings* settings)
    : clFileSystemWorkspaceDlgBase(parent)
    , m_settings(settings)
{
    Hide();
    if(m_settings == nullptr) {
        m_settings = &clFileSystemWorkspace::Get().GetSettings();
        m_usingGlobalSettings = true;
    } else {
        m_usingGlobalSettings = false;
    }

    const auto& configsMap = m_settings->GetConfigsMap();
    clFileSystemWorkspaceConfig::Ptr_t conf = m_settings->GetSelectedConfig();
    wxString selConf;
    if(conf) {
        selConf = conf->GetName();
    }
    for(const auto& vt : configsMap) {
        FSConfigPage* page = new FSConfigPage(m_notebook, vt.second, m_usingGlobalSettings);
        m_notebook->AddPage(page, vt.second->GetName(), false);
    }

    if(!selConf.empty()) {
        CallAfter(&clFileSystemWorkspaceDlg::SelectConfig, selConf);
    }

    ::clSetSmallDialogBestSizeAndPosition(this);
    CenterOnParent();
}

clFileSystemWorkspaceDlg::~clFileSystemWorkspaceDlg() {}

void clFileSystemWorkspaceDlg::SelectConfig(const wxString& config)
{
    for(size_t i = 0; i < m_notebook->GetPageCount(); ++i) {
        if(m_notebook->GetPageText(i) == config) {
            m_notebook->SetSelection(i);
        }
    }
}

void clFileSystemWorkspaceDlg::OnOK(wxCommandEvent& event)
{
    for(size_t i = 0; i < m_notebook->GetPageCount(); ++i) {
        FSConfigPage* page = dynamic_cast<FSConfigPage*>(m_notebook->GetPage(i));
        if(!page) {
            continue;
        }
        page->Save();
    }

    int sel = m_notebook->GetSelection();
    if(m_usingGlobalSettings) {
        clFileSystemWorkspace::Get().Save(false);
    }
    m_settings->SetSelectedConfig(m_notebook->GetPageText(sel));
    if(m_usingGlobalSettings) {
        clFileSystemWorkspace::Get().Save(true);
    }
    EndModal(wxID_OK);
}

void clFileSystemWorkspaceDlg::OnNewConfig(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clFSWNewConfigDlg dlg(this);
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    wxString name = dlg.GetConfigName();
    if(name.IsEmpty()) {
        return;
    }

    wxString copyFrom = dlg.GetCopyFrom();
    if(copyFrom == "-- None --") {
        copyFrom.Clear();
    }

    if(m_settings->AddConfig(name, copyFrom)) {
        clFileSystemWorkspaceConfig::Ptr_t conf = m_settings->GetConfig(name);
        FSConfigPage* page = new FSConfigPage(m_notebook, conf, m_usingGlobalSettings);
        m_notebook->AddPage(page, name, true);
    }
}

void clFileSystemWorkspaceDlg::OnDeleteConfig(wxCommandEvent& event)
{
    if(m_notebook->GetSelection() == wxNOT_FOUND) {
        return;
    }
    if(m_notebook->GetPageCount() == 1) {
        return;
    }
    int sel = m_notebook->GetSelection();
    if(sel == wxNOT_FOUND) {
        return;
    }

    wxString message;
    message << _("Choosing 'Yes' will delete workspace configuration '") << m_notebook->GetPageText(sel) << "'\n";
    message << _("Continue?");
    if(::wxMessageBox(message, "Confirm", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING, this) != wxYES) {
        return;
    }

    if(m_settings->DeleteConfig(m_notebook->GetPageText(sel))) {
        wxWindowUpdateLocker locker(this);
        m_notebook->DeletePage(sel);
    }
}

void clFileSystemWorkspaceDlg::OnDeleteConfigUI(wxUpdateUIEvent& event)
{
    event.Enable(m_notebook->GetPageCount() > 1);
}

void clFileSystemWorkspaceDlg::SetUseRemoteBrowsing(bool useRemoteBrowsing, const wxString& account)
{
    this->m_useRemoteBrowsing = useRemoteBrowsing;
    this->m_sshAccount = account;
    for(size_t i = 0; i < m_notebook->GetPageCount(); ++i) {
        FSConfigPage* page = reinterpret_cast<FSConfigPage*>(m_notebook->GetPage(i));
        if(!page) {
            continue;
        }
        page->SetUseRemoteBrowsing(useRemoteBrowsing, account);
    }
}
