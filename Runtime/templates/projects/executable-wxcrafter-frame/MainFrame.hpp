#ifndef MAINFRAME_HPP
#define MAINFRAME_HPP
#include "wxcrafter.hpp"

class MainFrame : public MainFrameBaseClass
{
public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();
protected:
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnExit(wxCommandEvent& event);
};
#endif // MAINFRAME_HPP
