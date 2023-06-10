#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "wxTerminalCtrl.h"
#include "wxcrafter.h"

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
    void OnTerminalExit(wxTerminalEvent& event);
    void OnSetTitle(wxTerminalEvent& event);
};
#endif // MAINFRAME_H
