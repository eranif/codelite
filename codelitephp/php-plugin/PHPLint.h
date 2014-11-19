#ifndef PHPLINT_H
#define PHPLINT_H

#include <wx/string.h>
#include <wx/event.h>
#include <list>
#include <wx/filename.h>
#include "smart_ptr.h"

class PhpPlugin;
class IProcess;
class PHPLint : public wxEvtHandler
{
    PhpPlugin* m_plugin;
    std::list<wxFileName> m_queue;
    wxString m_output;
    IProcess* m_process;
    wxString m_currentFileBeingProcessed;
    
public:
    typedef SmartPtr<PHPLint> Ptr_t;
    
protected:
    void DoProcessQueue();
    void DoCheckFile(const wxFileName& filename);
    
public:
    PHPLint(PhpPlugin *plugin);
    ~PHPLint();
    
    /**
     * @brief run php line on the current file and report back the error
     */
    void CheckCode(const wxFileName& filename);
    
    DECLARE_EVENT_TABLE()
    void OnProcessTerminated(wxCommandEvent &event);
    void OnProcessOutput(wxCommandEvent &event);
};

#endif // PHPLINT_H
