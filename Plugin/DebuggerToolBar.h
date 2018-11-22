#ifndef DEBUGGERTOOLBAR_H
#define DEBUGGERTOOLBAR_H

#include <wx/minifram.h>
#include "codelite_exports.h"
#include <wx/frame.h>
#include <wx/popupwin.h>
#include <wx/panel.h>

class clToolBar;
class WXDLLIMPEXP_SDK Gripper : public wxPanel
{
    wxPoint m_offset;

protected:
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnMotion(wxMouseEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnAppActivated(wxActivateEvent& event);

public:
    Gripper(wxWindow* parent);
    virtual ~Gripper();
};

class WXDLLIMPEXP_SDK DebuggerToolBar : public wxPanel
{
    clToolBar* m_tb = nullptr;
    Gripper* m_gripper = nullptr;

public:
    DebuggerToolBar(wxWindow* parent);
    virtual ~DebuggerToolBar();
    bool Show(bool show = true);
};

#endif // DEBUGGERTOOLBAR_H
