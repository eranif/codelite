#ifndef CLBITMAPOVERLAYCTRL_H
#define CLBITMAPOVERLAYCTRL_H

#include "codelite_exports.h"
#include "smart_ptr.h"

#include <wx/event.h>
#include <wx/statbmp.h>
#include <wx/string.h>
#include <wx/window.h>

class WXDLLIMPEXP_SDK clBitmapOverlayCtrl : public wxEvtHandler
{
    wxWindow* m_win;
    wxString m_name;
    size_t m_flags;
    wxStaticBitmap* m_bmpWindow;
    wxBitmap m_bmp;

public:
    typedef SmartPtr<clBitmapOverlayCtrl> Ptr_t;

protected:
    void DoPosition();
    void OnSize(wxSizeEvent& event);

public:
    clBitmapOverlayCtrl(wxWindow* win, const wxString& bitmapname, size_t flags);
    ~clBitmapOverlayCtrl();
};

#endif // CLBITMAPOVERLAYCTRL_H
