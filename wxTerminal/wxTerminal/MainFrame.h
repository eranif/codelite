#ifndef MAINFRAME_H
#define MAINFRAME_H
#include "wxcrafter.h"

class wxTerminalCtrl;
class MainFrame : public MainFrameBaseClass
{
    wxTerminalCtrl* m_terminal = nullptr;
public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};
#endif // MAINFRAME_H
