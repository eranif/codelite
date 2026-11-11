#pragma once

#include "popup_window_wrapper.h"

#include <wx/frame.h>

class PopupWindowPreview : public wxFrame
{
public:
    PopupWindowPreview(wxWindow* parent, const PopupWindowWrapper& wrapper);
    ~PopupWindowPreview() override;

    DECLARE_EVENT_TABLE()
    void OnClosePreviewPreviewPanel(wxCommandEvent& e);
};
