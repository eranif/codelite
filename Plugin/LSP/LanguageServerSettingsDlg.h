#ifndef LANGUAGESERVERSETTINGSDLG_H
#define LANGUAGESERVERSETTINGSDLG_H
#include "UI.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK LanguageServerSettingsDlg : public LanguageServerSettingsDlgBase
{
    bool m_scanOnStartup = false;

public:
    LanguageServerSettingsDlg(wxWindow* parent, bool triggerScan);
    virtual ~LanguageServerSettingsDlg() = default;
    void Save();

protected:
    virtual void OnButtonOK(wxCommandEvent& event);
    virtual void OnScan(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnDeleteLSP(wxCommandEvent& event);
    virtual void OnDeleteLSPUI(wxUpdateUIEvent& event);
    virtual void OnAddServer(wxCommandEvent& event);
    void DoInitialize();
    void DoScan();
};
#endif // LANGUAGESERVERSETTINGSDLG_H
