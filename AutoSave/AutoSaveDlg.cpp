#include "AutoSaveDlg.h"
#include "AutoSaveSettings.h"

AutoSaveDlg::AutoSaveDlg(wxWindow* parent)
    : AutoSaveDlgBase(parent)
{
    AutoSaveSettings conf = AutoSaveSettings::Load();
    m_checkBoxEnabled->SetValue(conf.HasFlag(AutoSaveSettings::kEnabled));
    m_spinCtrlInterval->SetValue(conf.GetCheckInterval());
}

AutoSaveDlg::~AutoSaveDlg() {}

void AutoSaveDlg::OnEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxEnabled->IsChecked()); }

void AutoSaveDlg::OnOK(wxCommandEvent& event)
{
    AutoSaveSettings conf;
    conf.EnableFlag(AutoSaveSettings::kEnabled, m_checkBoxEnabled->IsChecked());
    conf.SetCheckInterval(m_spinCtrlInterval->GetValue());
    AutoSaveSettings::Save(conf);
    EndModal(wxID_OK);
}
