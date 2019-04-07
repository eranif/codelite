#ifndef MAINFRAME_H
#define MAINFRAME_H
#include "wxcrafter.h"
#include "wxTerminalOptions.h"
#include <cl_command_event.h>

class wxTerminalCtrl;
class MainFrame : public MainFrameBaseClass
{
    wxTerminalCtrl* m_terminal = nullptr;
    wxTerminalOptions& m_options;

protected:
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnSettings(wxCommandEvent& event);
    void DoClose();

public:
    MainFrame(wxWindow* parent, wxTerminalOptions& options);
    virtual ~MainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTerminalExit(clCommandEvent& event);
};
#endif // MAINFRAME_H
