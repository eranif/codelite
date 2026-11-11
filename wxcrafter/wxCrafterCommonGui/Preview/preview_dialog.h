#pragma once

#include "dialog_wrapper.h"

#include <wx/dialog.h>

class PreviewDialog : public wxDialog
{

    DECLARE_EVENT_TABLE()
    void OnClose(wxCloseEvent& e);

public:
    PreviewDialog(wxWindow* parent, const DialogWrapper& dw);
    ~PreviewDialog() override;
    void OnClosePreview(wxCommandEvent& e);
};
