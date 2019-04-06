#ifndef MAINFRAME_H
#define MAINFRAME_H
#include "wxcrafter.h"
#include "wxTerminalOptions.h"

class wxTerminalCtrl;
class MainFrame : public MainFrameBaseClass
{
    wxTerminalCtrl* m_terminal = nullptr;
    wxTerminalOptions& m_options;

public:
    MainFrame(wxWindow* parent, wxTerminalOptions& options);
    virtual ~MainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};
#endif // MAINFRAME_H
