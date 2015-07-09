#ifndef CLTERNSERVER_H
#define CLTERNSERVER_H

#include "wx/event.h" // Base class: wxEvtHandler
#include <wx/filename.h>
#include <wx/arrstr.h>
#include "wxCodeCompletionBoxEntry.h"
#include "clTernWorkerThread.h"
#include "cl_calltip.h"
#include "json_node.h"
#include "cl_command_event.h"

class IEditor;
class wxStyledTextCtrl;
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
    bool m_goingDown;
    wxCodeCompletionBoxEntry::Vec_t m_entries;
    clTernWorkerThread* m_workerThread;
    bool m_fatalError;
    long m_port;
    size_t m_recycleCount;
    wxString m_workingDirectory;

protected:
    void OnTernTerminated(clProcessEvent& event);
    void OnTernOutput(clProcessEvent& event);
    void PrintMessage(const wxString& message);

    void ProcessOutput(const wxString& output, wxCodeCompletionBoxEntry::Vec_t& entries);
    clCallTipPtr ProcessCalltip(const wxString& output);

    wxString PrepareDoc(const wxString& doc, const wxString& url);
    void ProcessType(const wxString& type, wxString& signature, wxString& retValue, int& imgID);

    // Worker thread callbacks
    void OnTernWorkerThreadDone(const clTernWorkerThread::Reply& reply);
    void OnError(const wxString& why);
    JSONElement CreateLocation(wxStyledTextCtrl* ctrl, int pos = wxNOT_FOUND);
    JSONElement CreateFilesArray(wxStyledTextCtrl* ctrl);

public:
    void RecycleIfNeeded(bool force = false);
    clTernServer(JSCodeCompletion* cc);
    virtual ~clTernServer();

    long GetPort() const { return m_port; }
    bool Start(const wxString& workingDirectory);
    void Terminate();
    void ClearFatalErrorFlag();

    /**
     * @brief post a CC request at the current editor position
     */
    bool PostCCRequest(IEditor* editor);
    /**
     * @brief post a function calltip at a given position. pos is the first position
     * before the open brace
     */
    bool PostFunctionTipRequest(IEditor* editor, int pos);
    const wxCodeCompletionBoxEntry::Vec_t& GetEntries() const { return m_entries; }

    /**
     * @brief locate nodejs executable on this machine
     */
    static bool LocateNodeJS(wxFileName& nodeJS);
};

#endif // CLTERNSERVER_H
