#ifndef WORDCOMPLETIONSETTINGSDLG_H
#define WORDCOMPLETIONSETTINGSDLG_H
#include "UI.h"

class WordCompletionSettingsDlg : public WordCompletionSettingsBaseDlg
{
    bool m_modified;
public:
    WordCompletionSettingsDlg(wxWindow* parent);
    virtual ~WordCompletionSettingsDlg();
protected:
    virtual void OnOk(wxCommandEvent& event);
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnValueChanged(wxPropertyGridEvent& event);
};
#endif // WORDCOMPLETIONSETTINGSDLG_H
