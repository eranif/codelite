#ifndef CLTERNSERVER_H
#define CLTERNSERVER_H

#include "wx/event.h" // Base class: wxEvtHandler

class IProcess;
class clTernServer : public wxEvtHandler
{
    IProcess* m_tern;

protected:
    void OnTernTerminated(wxCommandEvent& event);
    void OnTernOutput(wxCommandEvent& event);
    
public:
    clTernServer();
    virtual ~clTernServer();
    
    void Start();
    void Terminate();
};

#endif // CLTERNSERVER_H
