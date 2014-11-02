#ifndef WORDCOMPLETIONSETTINGSDLG_H
#define WORDCOMPLETIONSETTINGSDLG_H
#include "UI.h"

class WordCompletionSettingsDlg : public WordCompletionSettingsBaseDlg
{
public:
    WordCompletionSettingsDlg(wxWindow* parent);
    virtual ~WordCompletionSettingsDlg();
protected:
    virtual void OnValueChanged(wxPropertyGridEvent& event);
};
#endif // WORDCOMPLETIONSETTINGSDLG_H
