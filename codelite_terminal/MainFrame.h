#ifndef MAINFRAME_H
#define MAINFRAME_H
#include "wxcrafter.h"

#include <cl_command_event.h>

class wxTerminalCtrl;
class MainFrame : public MainFrameBaseClass
{
    wxTerminalCtrl* m_terminal = nullptr;

protected:
    virtual void OnButton93ButtonClicked(wxCommandEvent& event);
    virtual void OnClearScreen(wxCommandEvent& event);
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnSettings(wxCommandEvent& event);
    void DoClose();

public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTerminalExit(clCommandEvent& event);
    void OnSetTitle(clCommandEvent& event);
};
#endif // MAINFRAME_H
