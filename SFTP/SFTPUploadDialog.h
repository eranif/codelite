#ifndef SFTPUPLOADDIALOG_H
#define SFTPUPLOADDIALOG_H
#include "UI.h"

class SFTPUploadDialog : public SFTPUploadDialogBase
{
public:
    SFTPUploadDialog(wxWindow* parent);
    virtual ~SFTPUploadDialog();
protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // SFTPUPLOADDIALOG_H
