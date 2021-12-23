#include "clFSWNewConfigDlg.h"

#include "clFileSystemWorkspace.hpp"

clFSWNewConfigDlg::clFSWNewConfigDlg(wxWindow* parent)
    : clFSWNewConfigDlgBase(parent)
{
    auto& s = clFileSystemWorkspace::Get().GetSettings();
    auto configs = s.GetConfigs();
    configs.Insert("-- None --", 0);
    m_choiceCopyFrom->Append(configs);
    auto conf = s.GetSelectedConfig();
    if(conf && m_choiceCopyFrom->FindString(conf->GetName())) {
        m_choiceCopyFrom->SetStringSelection(conf->GetName());
    } else if(!m_choiceCopyFrom->IsEmpty()) {
        m_choiceCopyFrom->SetSelection(0);
    }
}

clFSWNewConfigDlg::~clFSWNewConfigDlg() {}

void clFSWNewConfigDlg::OnOKUI(wxUpdateUIEvent& event) { event.Enable(!m_textCtrlName->GetValue().IsEmpty()); }
wxString clFSWNewConfigDlg::GetCopyFrom() const { return m_choiceCopyFrom->GetStringSelection(); }
wxString clFSWNewConfigDlg::GetConfigName() const { return m_textCtrlName->GetValue(); }
