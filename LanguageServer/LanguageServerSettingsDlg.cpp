#include "LanguageServerSettingsDlg.h"

#include "LSPDetectorManager.hpp"
#include "LanguageServerConfig.h"
#include "LanguageServerEntry.h"
#include "LanguageServerPage.h"
#include "NewLanguageServerDlg.h"
#include "detectors/LSPDetector.hpp"
#include "globals.h"

#include <vector>
#include <wx/choicdlg.h>
#include <wx/msgdlg.h>
#include <wx/wupdlock.h>

LanguageServerSettingsDlg::LanguageServerSettingsDlg(wxWindow* parent, bool triggerScan)
    : LanguageServerSettingsDlgBase(parent)
    , m_scanOnStartup(triggerScan)
{
    DoInitialize();
    ::clSetDialogBestSizeAndPosition(this);
    if(m_scanOnStartup) {
        CallAfter(&LanguageServerSettingsDlg::DoScan);
    }
}

LanguageServerSettingsDlg::~LanguageServerSettingsDlg() {}

void LanguageServerSettingsDlg::OnAddServer(wxCommandEvent& event)
{
    NewLanguageServerDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        LanguageServerEntry server = dlg.GetData();
        // Update the configuration
        LanguageServerConfig::Get().AddServer(server);
        m_notebook->AddPage(new LanguageServerPage(m_notebook, server), server.GetName());
    }
}

void LanguageServerSettingsDlg::Save()
{
    LanguageServerConfig& conf = LanguageServerConfig::Get();
    for(size_t i = 0; i < m_notebook->GetPageCount(); ++i) {
        LanguageServerPage* page = dynamic_cast<LanguageServerPage*>(m_notebook->GetPage(i));
        conf.AddServer(page->GetData());
    }
    conf.SetEnabled(GetCheckBoxEnable()->IsChecked());
    conf.Save();
}

void LanguageServerSettingsDlg::OnDeleteLSP(wxCommandEvent& event)
{
    int sel = m_notebook->GetSelection();
    if(sel == wxNOT_FOUND) {
        return;
    }
    wxString serverName = m_notebook->GetPageText(sel);

    if(::wxMessageBox(wxString() << _("Are you sure you want to delete '") << serverName << "' ?", "CodeLite",
                      wxICON_QUESTION | wxCENTRE | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT, this) != wxYES) {
        return;
    }
    LanguageServerConfig::Get().RemoveServer(serverName);
    m_notebook->DeletePage(sel);
}

void LanguageServerSettingsDlg::OnDeleteLSPUI(wxUpdateUIEvent& event) { event.Enable(m_notebook->GetPageCount()); }
void LanguageServerSettingsDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(true); }

void LanguageServerSettingsDlg::OnScan(wxCommandEvent& event)
{
    event.Skip();
    if(::wxMessageBox(_("This will reconfigure your language servers\nContinue?"), "CodeLite",
                      wxICON_QUESTION | wxYES_NO | wxCANCEL | wxYES_DEFAULT) != wxYES) {
        return;
    }
    DoScan();
}

void LanguageServerSettingsDlg::DoInitialize()
{
    wxWindowUpdateLocker locker{ this };
    m_notebook->DeleteAllPages();
    const auto& servers = LanguageServerConfig::Get().GetServers();
    for(const auto& [name, server] : servers) {
        m_notebook->AddPage(new LanguageServerPage(m_notebook, server), server.GetName());
    }
    m_checkBoxEnable->SetValue(LanguageServerConfig::Get().IsEnabled());
}

void LanguageServerSettingsDlg::DoScan()
{
    // scan and replace the current servers with the what CodeLite can locate
    wxBusyCursor bc;
    std::vector<LSPDetector::Ptr_t> matches;
    LSPDetectorManager detector;
    if(detector.Scan(matches)) {
        LanguageServerConfig& conf = LanguageServerConfig::Get();
        LanguageServerEntry::Map_t servers;
        for(const auto& match : matches) {
            LanguageServerEntry entry;
            match->GetLanguageServerEntry(entry);
            servers.insert({ entry.GetName(), entry });
        }
        conf.SetServers(servers);
        conf.Save();
        DoInitialize();
        if(m_scanOnStartup) {
            m_checkBoxEnable->SetValue(true);
        }
    }
}
void LanguageServerSettingsDlg::OnButtonOK(wxCommandEvent& event)
{
    // validate the data
    for(size_t i = 0; i < m_notebook->GetPageCount(); ++i) {
        LanguageServerPage* page = dynamic_cast<LanguageServerPage*>(m_notebook->GetPage(i));
        wxString message;
        if(!page->ValidateData(&message)) {
            ::wxMessageBox(message, "CodeLite", wxOK | wxCENTRE | wxICON_WARNING, this);
            event.Skip(false);
            return;
        }
    }
    event.Skip();
}
