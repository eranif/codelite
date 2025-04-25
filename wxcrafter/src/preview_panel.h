#ifndef PREVIEWPANEL_H
#define PREVIEWPANEL_H

#include "frame_wrapper.h"

#include <wx/frame.h> // Base class: wxFrame

class PreviewPanel : public wxFrame
{
public:
    PreviewPanel(wxWindow* parent, const TopLevelWinWrapper& fw);
    virtual ~PreviewPanel();
    DECLARE_EVENT_TABLE()
    void OnClosePreviewPreviewPanel(wxCommandEvent& e);
};

#endif // PREVIEWPANEL_H
