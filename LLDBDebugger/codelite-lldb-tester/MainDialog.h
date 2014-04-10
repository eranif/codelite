#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "wxcrafter.h"
#include "LLDBProtocol/LLDBConnector.h"
#include "LLDBProtocol/LLDBEvent.h"

class MainDialog : public MainDialogBaseClass
{
    LLDBConnector m_connector;
    
public:
    MainDialog(wxWindow* parent);
    virtual ~MainDialog();
    void LogCommand( const LLDBCommand & command );
    
protected:
    virtual void OnStart(wxCommandEvent& event);
    virtual void OnStop(wxCommandEvent& event);
    
    void OnLLDBStarted(LLDBEvent &e);
    void OnLLDBRunning(LLDBEvent &e);
    void OnLLDBExited(LLDBEvent &e);
    void OnLLDBStoppedOnFirstEntry(LLDBEvent &e);
    void OnLLDBStopped(LLDBEvent &e);
    void OnLLDBBreakpointsUpdated(LLDBEvent &e);
    
};
#endif // MAINDIALOG_H
