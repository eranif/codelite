#ifndef LANGUAGESERVERSETTINGSDLG_H
#define LANGUAGESERVERSETTINGSDLG_H
#include "UI.h"

class LanguageServerSettingsDlg : public LanguageServerSettingsDlgBase
{
public:
    LanguageServerSettingsDlg(wxWindow* parent);
    virtual ~LanguageServerSettingsDlg();
    void Save();
    
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnDeleteLSP(wxCommandEvent& event);
    virtual void OnDeleteLSPUI(wxUpdateUIEvent& event);
    virtual void OnAddServer(wxCommandEvent& event);
};
#endif // LANGUAGESERVERSETTINGSDLG_H
