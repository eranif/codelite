#ifndef PREVIEWFRAME_H
#define PREVIEWFRAME_H

#include "frame_wrapper.h"
#include <wx/frame.h> // Base class: wxFrame

class PreviewFrame : public wxFrame
{
public:
    PreviewFrame(wxWindow* parent, const TopLevelWinWrapper& fw);
    virtual ~PreviewFrame();
    DECLARE_EVENT_TABLE()

    void OnClosePreview(wxCommandEvent& e);
};

#endif // PREVIEWFRAME_H
