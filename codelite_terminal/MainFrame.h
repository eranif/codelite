#ifndef MAINFRAME_H
#define MAINFRAME_H
#include "wxcrafter.h"
#include "my_callback.h"

class IProcess;
class MainFrame : public MainFrameBaseClass
{
    friend class MyCallback;

    IProcess *m_process;
    MyCallback m_callback;
    int m_fromPos;

protected:
    virtual void OnStcUpdateUI(wxStyledTextEvent& event);
    void DoExecuteCurrentLine();
    wxString GetCurrentLine() const;
    void SetCartAtEnd();
    void AppendNewLine();
    
public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

protected:
    virtual void OnKeyDown(wxKeyEvent& event);
};
#endif // MAINFRAME_H
