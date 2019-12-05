#ifndef LANGUAGESERVERSETTINGSDLG_H
#define LANGUAGESERVERSETTINGSDLG_H
#include "UI.h"

class LanguageServerSettingsDlg : public LanguageServerSettingsDlgBase
{
    bool m_scanOnStartup = false;

public:
    LanguageServerSettingsDlg(wxWindow* parent, bool triggerScan);
    virtual ~LanguageServerSettingsDlg();
    void Save();

protected:
    virtual void OnScan(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnDeleteLSP(wxCommandEvent& event);
    virtual void OnDeleteLSPUI(wxUpdateUIEvent& event);
    virtual void OnAddServer(wxCommandEvent& event);
    void DoInitialize();
    void DoScan();
};
#endif // LANGUAGESERVERSETTINGSDLG_H
