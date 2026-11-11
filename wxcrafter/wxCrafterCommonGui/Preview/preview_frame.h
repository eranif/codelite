#pragma once

#include "frame_wrapper.h"

#include <wx/frame.h> // Base class: wxFrame

class PreviewFrame : public wxFrame
{
public:
    PreviewFrame(wxWindow* parent, const TopLevelWinWrapper& fw);
    ~PreviewFrame() override;
    DECLARE_EVENT_TABLE()

    void OnClosePreview(wxCommandEvent& e);
};
