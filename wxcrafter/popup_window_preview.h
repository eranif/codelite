#ifndef POPUPWINDOWPREVIEW_H
#define POPUPWINDOWPREVIEW_H

#include "popup_window_wrapper.h"
#include <wx/frame.h>
#include <wx/popupwin.h>

class PopupWindowPreview : public wxFrame
{
public:
    PopupWindowPreview(wxWindow* parent, const PopupWindowWrapper& wrapper);
    virtual ~PopupWindowPreview();

    DECLARE_EVENT_TABLE()
    void OnClosePreviewPreviewPanel(wxCommandEvent& e);
};

#endif // POPUPWINDOWPREVIEW_H
