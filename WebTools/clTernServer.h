#ifndef CLTERNSERVER_H
#define CLTERNSERVER_H

#include "wx/event.h" // Base class: wxEvtHandler
#include <wx/filename.h>
#include <wx/arrstr.h>
#include "wxCodeCompletionBoxEntry.h"

class JSCodeCompletion;
class IProcess;
class clTernWorkerThread;
class clTernServer : public wxEvtHandler
{
    friend class clTernWorkerThread;
    
    JSCodeCompletion* m_jsCCManager;
    IProcess* m_tern;
    wxFileName m_nodePath;
    wxString m_ternBinFolder;
    int m_port;
    bool m_goingDown;
    wxArrayString m_tempfiles;
    wxCodeCompletionBoxEntry::Vec_t m_entries;
    clTernWorkerThread* m_workerThread;
    
protected:
    DECLARE_EVENT_TABLE()
    void OnTernTerminated(wxCommandEvent& event);
    void OnTernOutput(wxCommandEvent& event);
    void PrintMessage(const wxString& message);

    void RecycleIfNeeded();
    void ProcessOutput(const wxString& output, wxCodeCompletionBoxEntry::Vec_t& entries);
    wxString PrepareDoc(const wxString& doc, const wxString& url);
    void ProcessType(const wxString& type, wxString& signature, wxString& retValue, int& imgID);
    
    // Worker thread callbacks
    void OnTernWorkerThreadDone(const wxString& json, const wxString& filename);
    void OnError(const wxString& why);
    
public:
    clTernServer(JSCodeCompletion* cc);
    virtual ~clTernServer();

    bool Start();
    void Terminate();
    void SetPort(int port) { this->m_port = port; }
    int GetPort() const { return m_port; }

    bool PostRequest(const wxString& request, const wxFileName& filename, const wxFileName& tmpFileName);
    const wxCodeCompletionBoxEntry::Vec_t& GetEntries() const { return m_entries; }
};

#endif // CLTERNSERVER_H
