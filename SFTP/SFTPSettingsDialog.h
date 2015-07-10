#ifndef SFTPSETTINGSDIALOG_H
#define SFTPSETTINGSDIALOG_H
#include "UI.h"

class SFTPSettingsDialog : public SFTPSettingsDialogBase
{
public:
    SFTPSettingsDialog(wxWindow* parent);
    virtual ~SFTPSettingsDialog();
protected:
    virtual void OnOK(wxCommandEvent& event);
};
#endif // SFTPSETTINGSDIALOG_H
