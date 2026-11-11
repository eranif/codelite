#pragma once

#include "frame_wrapper.h"

#include <wx/frame.h> // Base class: wxFrame

class PreviewPanel : public wxFrame
{
public:
    PreviewPanel(wxWindow* parent, const TopLevelWinWrapper& fw);
    ~PreviewPanel() override;
    DECLARE_EVENT_TABLE()
    void OnClosePreviewPreviewPanel(wxCommandEvent& e);
};
