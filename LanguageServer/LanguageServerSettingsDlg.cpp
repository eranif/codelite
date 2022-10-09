#include "LanguageServerSettingsDlg.h"

#include "LSPDetector.hpp"
#include "LSPDetectorManager.hpp"
#include "LanguageServerConfig.h"
#include "LanguageServerEntry.h"
#include "LanguageServerPage.h"
#include "NewLanguageServerDlg.h"
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
    DoScan();
}

void LanguageServerSettingsDlg::DoInitialize()
{
    wxWindowUpdateLocker locker{ this };
    m_notebook->DeleteAllPages();
    const LanguageServerEntry::Map_t& servers = LanguageServerConfig::Get().GetServers();
    for(const LanguageServerEntry::Map_t::value_type& vt : servers) {
        m_notebook->AddPage(new LanguageServerPage(m_notebook, vt.second), vt.second.GetName());
    }
    m_checkBoxEnable->SetValue(LanguageServerConfig::Get().IsEnabled());
}

void LanguageServerSettingsDlg::DoScan()
{
    wxBusyCursor bc;
    std::vector<LSPDetector::Ptr_t> matches;
    LSPDetectorManager detector;
    if(detector.Scan(matches)) {
        // Matches were found, reload the dialog
        // Prompt the user to select which entries to add
        wxArrayString options;
        wxArrayInt selections;
        for(size_t i = 0; i < matches.size(); ++i) {
            options.Add(matches[i]->GetName());
            selections.Add(i);
        }
        if((wxGetSelectedChoices(selections, _("Select Language Servers to add"), "CodeLite", options) ==
            wxNOT_FOUND) ||
           selections.empty()) {
            return;
        }
        LanguageServerConfig& conf = LanguageServerConfig::Get();
        for(size_t i = 0; i < selections.size(); ++i) {
            LanguageServerEntry entry;
            matches[selections[i]]->GetLanguageServerEntry(entry);
            conf.AddServer(entry);
        }
        conf.Save();
        DoInitialize();
        if(m_scanOnStartup) {
            m_checkBoxEnable->SetValue(true);
        }
    }
}
