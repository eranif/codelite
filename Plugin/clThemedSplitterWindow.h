#pragma once

#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/splitter.h>

class WXDLLIMPEXP_SDK clThemedSplitterWindow : public wxSplitterWindow
{
public:
    clThemedSplitterWindow() = default;
    clThemedSplitterWindow(wxWindow* parent,
                           wxWindowID id = wxID_ANY,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           long style = wxSP_3D,
                           const wxString& name = "splitterWindow");
    ~clThemedSplitterWindow() override = default;
    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_3D,
                const wxString& name = "splitterWindow");
};

