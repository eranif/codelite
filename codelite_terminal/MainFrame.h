#ifndef MAINFRAME_H
#define MAINFRAME_H
#include "wxcrafter.h"
#include "my_callback.h"
#include "my_callback.h"
#include "terminal_options.h"
#include "my_config.h"

class IProcess;
class MainFrame : public MainFrameBaseClass
{
    friend class MyCallback;
    friend class PtyCallback;

    IProcess *      m_process;
    PtyCallback     m_ptyCllback;
    int             m_fromPos;
    IProcess *      m_dummyProcess;
    wxString        m_tty;
    int             m_slave;
    TerminalOptions m_options;
    bool            m_exitOnNextKey;
    MyConfig        m_config;
    wxString        m_outoutBuffer;
protected:
    virtual void OnIdle(wxIdleEvent& event);
    virtual void OnSaveContentUI(wxUpdateUIEvent& event);
    virtual void OnSaveContent(wxCommandEvent& event);
    virtual void OnSettings(wxCommandEvent& event);
    virtual void OnSignalInferiorUI(wxUpdateUIEvent& event);
    virtual void OnSignalinferior(wxAuiToolBarEvent& event);
    virtual void OnClearViewUI(wxUpdateUIEvent& event);
    virtual void OnTerminateInfirior(wxCommandEvent& event);
    virtual void OnTerminateInfiriorUI(wxUpdateUIEvent& event);
    virtual void OnClearView(wxCommandEvent& event);
    virtual void OnAddMarker(wxTimerEvent& event);
    virtual void OnStcUpdateUI(wxStyledTextEvent& event);

    void OnSelectBgColour(wxCommandEvent &e);
    void OnSelectFgColour(wxCommandEvent &e);
    void OnSignal(wxCommandEvent &e);

    void DoExecuteCurrentLine(const wxString &command = wxEmptyString);
    wxString GetCurrentLine() const;
    void SetCartAtEnd();
    void AppendNewLine();
    wxString StartTTY();
    void StopTTY();
    void DoExecStartCommand();
    void DoSetColour(const wxColour& colour, bool bgColour = false);
    void DoSetFont(wxFont font);
    void DoApplySettings();
    void AppendOutputText(const wxString &text);
    void FlushOutputBuffer();
    
public:
    MainFrame(wxWindow* parent, const TerminalOptions &options, long style = wxDEFAULT_FRAME_STYLE);
    virtual ~MainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    const TerminalOptions& GetOptions() const {
        return m_options;
    }

    void Exit();

protected:
    virtual void OnKeyDown(wxKeyEvent& event);
};
#endif // MAINFRAME_H
