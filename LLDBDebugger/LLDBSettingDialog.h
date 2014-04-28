#ifndef LLDBSETTINGDIALOG_H
#define LLDBSETTINGDIALOG_H
#include "UI.h"

class LLDBSettingDialog : public LLDBSettingDialogBase
{
    bool m_modified;
public:
    LLDBSettingDialog(wxWindow* parent);
    virtual ~LLDBSettingDialog();
    void Save();
    
protected:
    virtual void OnApply(wxCommandEvent& event);
    virtual void OnAdvancedValueChanged(wxPropertyGridEvent& event);
    virtual void OnGeneralValueChanged(wxPropertyGridEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // LLDBSETTINGDIALOG_H
