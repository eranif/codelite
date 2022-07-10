#ifndef DAPOUTPUTVIEW_H
#define DAPOUTPUTVIEW_H

#include "UI.h"
#include "clModuleLogger.hpp"
#include "clTerminalViewCtrl.hpp"
#include "dap/dap.hpp"

#include <wx/stc/stc.h>

class DAPTerminalCtrlView : public wxPanel
{
protected:
    clModuleLogger& LOG;
    wxStyledTextCtrl* m_ctrl = nullptr;

protected:
    void OnThemeChanged(clCommandEvent& event);
    void ApplyTheme();

public:
    DAPTerminalCtrlView(wxWindow* parent, clModuleLogger& log);
    virtual ~DAPTerminalCtrlView();

    void AppendLine(const wxString& line);
    void ScrollToEnd();
};
#endif // DAPOUTPUTVIEW_H
