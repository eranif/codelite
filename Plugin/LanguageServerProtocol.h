#ifndef LANGUAG_ESERVER_PROTOCOL_H
#define LANGUAG_ESERVER_PROTOCOL_H

#include "codelite_exports.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include "LSP/clJSONRPC.h"
#include <wxStringHash.h>
#include <wx/sharedptr.h>
#include "macros.h"
#include <map>
#include <queue>
#include <string>
#include "LSP/RequestMessage.h"
#include <unordered_map>
#include "ChildProcess.h"

class IEditor;
class WXDLLIMPEXP_SDK LSPRequestMessageQueue
{
    std::queue<LSP::RequestMessage::Ptr_t> m_Queue;
    std::unordered_map<int, LSP::RequestMessage::Ptr_t> m_pendingReplyMessages;
    bool m_waitingReponse = false;

public:
    LSPRequestMessageQueue() {}
    virtual ~LSPRequestMessageQueue() {}
    
    LSP::RequestMessage::Ptr_t TakePendingReplyMessage(int msgid);
    void Push(LSP::RequestMessage::Ptr_t message);
    void Pop();
    LSP::RequestMessage::Ptr_t Get();
    void Clear();
    bool IsEmpty() const { return m_Queue.empty(); }
    void SetWaitingReponse(bool waitingReponse) { this->m_waitingReponse = waitingReponse; }
    bool IsWaitingReponse() const { return m_waitingReponse; }
};

class WXDLLIMPEXP_SDK LanguageServerProtocol : public wxEvtHandler
{
    enum eState {
        kUnInitialized,
        kInitialized,
    };

    wxString m_name;
    wxEvtHandler* m_owner = nullptr;
    ChildProcess::Ptr_t m_process;
    wxArrayString m_command;
    wxString m_workingDirectory;
    bool m_goingDown = false;
    wxStringSet_t m_filesSent;
    wxStringSet_t m_languages;
    wxString m_outputBuffer;
    wxString m_workspaceFolder;

    // initialization
    eState m_state = kUnInitialized;
    int m_initializeRequestID = wxNOT_FOUND;

    // Parsing queue
    LSPRequestMessageQueue m_Queue;

public:
    typedef wxSharedPtr<LanguageServerProtocol> Ptr_t;

protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessStderr(clProcessEvent& event);
    void OnFileLoaded(clCommandEvent& event);
    void OnFileClosed(clCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void OnWorkspaceLoaded(wxCommandEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);

protected:
    void DoClear();
    bool ShouldHandleFile(const wxFileName& fn) const;
    bool ShouldHandleFile(IEditor* editor) const;
    wxString GetLogPrefix() const;
    void ProcessQueue();
    static wxString GetLanguageId(const wxFileName& fn) { return GetLanguageId(fn.GetFullName()); }
    static wxString GetLanguageId(const wxString& fn);

protected:
    /**
     * @brief notify about file open
     */
    void SendOpenRequest(const wxFileName& filename, const wxString& fileContent, const wxString& languageId);

    /**
     * @brief report a file-close notification
     */
    void SendCloseRequest(const wxFileName& filename);

    /**
     * @brief report a file-changed notification
     */
    void SendChangeRequest(const wxFileName& filename, const wxString& fileContent);

    /**
     * @brief report a file-save notification
     */
    void SendSaveRequest(const wxFileName& filename, const wxString& fileContent);

    /**
     * @brief request for a code completion at a given doc/position
     */
    void SendCodeCompleteRequest(const wxFileName& filename, size_t line, size_t column);

    void DoStart();

    /**
     * @brief add message to the outgoing queue
     */
    void QueueMessage(LSP::RequestMessage::Ptr_t request);

public:
    LanguageServerProtocol(const wxString& name, wxEvtHandler* owner);
    virtual ~LanguageServerProtocol();

    LanguageServerProtocol& SetName(const wxString& name)
    {
        this->m_name = name;
        return *this;
    }

    const wxString& GetName() const { return m_name; }
    bool IsInitialized() const { return (m_state == kInitialized); }

    /**
     * @brief return list of all supported languages by LSP. The list contains the abbreviation entry and a description
     */
    static const std::set<wxString>& GetSupportedLanguages();
    bool CanHandle(const wxFileName& filename) const;

    /**
     * @brief start a server for an executable
     */
    void Start(const wxArrayString& command, const wxString& workingDirectory, const wxArrayString& languages);

    /**
     * @brief is the LSP running?
     */
    bool IsRunning() const;

    /**
     * @brief stop the language server
     */
    void Stop(bool goingDown);

    /**
     * @brief restart the server. If the server is not running, start it
     */
    void Restart();

    /**
     * @brief find the definition of the item at the caret position
     */
    void FindDefinition(IEditor* editor);

    /**
     * @brief perform code completion for a given editor
     */
    void CodeComplete(IEditor* editor);

    /**
     * @brief manually load file into the server
     */
    void OpenEditor(IEditor* editor);
    
    /**
     * @brief tell the server to close editor
     */
    void CloseEditor(IEditor* editor);
};

#endif // CLLANGUAGESERVER_H
