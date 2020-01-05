#ifndef LANGUAG_ESERVER_PROTOCOL_H
#define LANGUAG_ESERVER_PROTOCOL_H

#include "LSP/IPathConverter.hpp"
#include "LSP/MessageWithParams.h"
#include "LSPNetwork.h"
#include "ServiceProvider.h"
#include "SocketAPI/clSocketClientAsync.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "macros.h"
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <wx/filename.h>
#include <wx/sharedptr.h>
#include <wxStringHash.h>

class IEditor;
class WXDLLIMPEXP_SDK LSPRequestMessageQueue
{
    std::queue<LSP::MessageWithParams::Ptr_t> m_Queue;
    std::unordered_map<int, LSP::MessageWithParams::Ptr_t> m_pendingReplyMessages;
    bool m_waitingReponse = false;

public:
    LSPRequestMessageQueue() {}
    virtual ~LSPRequestMessageQueue() {}

    LSP::MessageWithParams::Ptr_t TakePendingReplyMessage(int msgid);
    void Push(LSP::MessageWithParams::Ptr_t message);
    void Pop();
    LSP::MessageWithParams::Ptr_t Get();
    void Clear();
    bool IsEmpty() const { return m_Queue.empty(); }
    void SetWaitingReponse(bool waitingReponse) { this->m_waitingReponse = waitingReponse; }
    bool IsWaitingReponse() const { return m_waitingReponse; }
};

class WXDLLIMPEXP_SDK LanguageServerProtocol : public ServiceProvider
{
    enum eState {
        kUnInitialized,
        kInitialized,
    };

    wxString m_name;
    wxEvtHandler* m_owner = nullptr;
    LSPNetwork::Ptr_t m_network;
    wxArrayString m_lspCommand;
    wxString m_workingDirectory;
    wxStringSet_t m_filesSent;
    wxStringSet_t m_languages;
    wxString m_outputBuffer;
    wxString m_rootFolder;
    wxString m_connectionString;
    IPathConverter::Ptr_t m_pathConverter;

    // initialization
    eState m_state = kUnInitialized;
    int m_initializeRequestID = wxNOT_FOUND;

    // Parsing queue
    LSPRequestMessageQueue m_Queue;
    size_t m_createFlags = 0;
    wxStringSet_t m_unimplementedMethods;
    bool m_disaplayDiagnostics = true;
    int m_lastCompletionRequestId = wxNOT_FOUND;

public:
    typedef wxSharedPtr<LanguageServerProtocol> Ptr_t;

protected:
    void OnNetConnected(clCommandEvent& event);
    void OnNetError(clCommandEvent& event);
    void OnNetDataReady(clCommandEvent& event);

    void OnFileLoaded(clCommandEvent& event);
    void OnFileClosed(clCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void OnEditorChanged(wxCommandEvent& event);
    void OnCodeComplete(clCodeCompletionEvent& event);
    void OnFindSymbolDecl(clCodeCompletionEvent& event);
    void OnFindSymbolImpl(clCodeCompletionEvent& event);
    void OnFindSymbol(clCodeCompletionEvent& event);
    void OnFunctionCallTip(clCodeCompletionEvent& event);

protected:
    void DoClear();
    bool ShouldHandleFile(const wxFileName& fn) const;
    bool ShouldHandleFile(IEditor* editor) const;
    wxString GetLogPrefix() const;
    void ProcessQueue();
    static wxString GetLanguageId(const wxFileName& fn);
    static wxString GetLanguageId(const wxString& fn);

protected:
    /**
     * @brief notify about file open
     */
    void SendOpenRequest(const wxFileName& filename, const std::string& fileContent, const wxString& languageId);

    /**
     * @brief report a file-close notification
     */
    void SendCloseRequest(const wxFileName& filename);

    /**
     * @brief report a file-changed notification
     */
    void SendChangeRequest(const wxFileName& filename, const std::string& fileContent);

    /**
     * @brief report a file-save notification
     */
    void SendSaveRequest(const wxFileName& filename, const std::string& fileContent);

    /**
     * @brief request for a code completion at a given doc/position
     */
    void SendCodeCompleteRequest(const wxFileName& filename, size_t line, size_t column);

    bool DoStart();

    /**
     * @brief add message to the outgoing queue
     */
    void QueueMessage(LSP::MessageWithParams::Ptr_t request);

public:
    LanguageServerProtocol(const wxString& name, eNetworkType netType, wxEvtHandler* owner,
                           IPathConverter::Ptr_t pathConverter);
    virtual ~LanguageServerProtocol();

    LanguageServerProtocol& SetDisaplayDiagnostics(bool disaplayDiagnostics)
    {
        this->m_disaplayDiagnostics = disaplayDiagnostics;
        return *this;
    }
    bool IsDisaplayDiagnostics() const { return m_disaplayDiagnostics; }
    void SetPathConverter(IPathConverter::Ptr_t pathConverter) { this->m_pathConverter = pathConverter; }
    IPathConverter::Ptr_t GetPathConverter() const { return m_pathConverter; }
    LanguageServerProtocol& SetName(const wxString& name)
    {
        this->m_name = name;
        return *this;
    }

    LanguageServerProtocol& SetUnimplementedMethods(const wxStringSet_t& unimplementedMethods)
    {
        this->m_unimplementedMethods = unimplementedMethods;
        return *this;
    }
    const wxStringSet_t& GetUnimplementedMethods() const { return m_unimplementedMethods; }
    const wxString& GetName() const { return m_name; }
    bool IsInitialized() const { return (m_state == kInitialized); }

    /**
     * @brief return list of all supported languages by LSP. The list contains the abbreviation entry and a description
     */
    static std::set<wxString> GetSupportedLanguages();
    bool CanHandle(const wxFileName& filename) const;

    /**
     * @brief start LSP server and connect to it (e.g. clangd)
     * @param lspCommand LSP server command
     * @param connectionString
     * @param rootFolder the LSP root folder (to be passed during the 'initialize' request)
     * @param languages supported languages by this LSP
     */
    bool Start(const wxArrayString& lspCommand, const wxString& connectionString, const wxString& workingDirectory,
               const wxString& rootFolder, const wxArrayString& languages, size_t flags);

    /**
     * @brief same as above, but reuse the current parameters
     */
    bool Start();

    /**
     * @brief is the LSP running?
     */
    bool IsRunning() const;

    /**
     * @brief stop the language server
     */
    void Stop();

    /**
     * @brief find the definition of the item at the caret position
     */
    void FindDefinition(IEditor* editor);
    /**
     * @brief find the implementatin of a symbol at the caret position
     */
    void FindImplementation(IEditor* editor);
    /**
     * @brief find the definition of the item at the caret position
     */
    void FindDeclaration(IEditor* editor);

    /**
     * @brief perform code completion for a given editor
     */
    void CodeComplete(IEditor* editor);

    /**
     * @brief ask for function call help
     */
    void FunctionHelp(IEditor* editor);

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
