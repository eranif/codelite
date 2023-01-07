#ifndef MYFRAME_H
#define MYFRAME_H

#include "wxc_aui_manager.h"
#include <wx/frame.h>

class MyFrame : public wxFrame
{
public:
    MyFrame();
    MyFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxFrameNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }
    virtual ~MyFrame();
};

#endif // MYFRAME_H
