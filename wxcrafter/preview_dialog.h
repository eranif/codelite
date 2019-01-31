#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include "dialog_wrapper.h"
#include <wx/dialog.h>

class PreviewDialog : public wxDialog
{

    DECLARE_EVENT_TABLE()
    void OnClose(wxCloseEvent& e);

public:
    PreviewDialog(wxWindow* parent, const DialogWrapper& dw);
    virtual ~PreviewDialog();
    void OnClosePreview(wxCommandEvent& e);
};

#endif // PREVIEWDIALOG_H
