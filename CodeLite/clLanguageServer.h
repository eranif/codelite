#ifndef CLLANGUAGESERVER_H
#define CLLANGUAGESERVER_H

#include "codelite_exports.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include "json_rpc/clJSONRPC.h"

class WXDLLIMPEXP_CL clLanguageServer : public json_rpc::Sender
{
    IProcess* m_process = NULL;
    wxString m_command;
    wxString m_workingDirectory;
    bool m_goingDown = false;

protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);

public:
    clLanguageServer();
    virtual ~clLanguageServer();

    /**
     * @brief pure method
     */
    virtual void Send(const wxString& message);

    /**
     * @brief start a server for an executable
     */
    void Start(const wxString& command, const wxString& workingDirectory);

    /**
     * @brief stop the language server
     */
    void Stop();
    
    /**
     * @brief find definition of a word.
     * @param filename the filename
     * @param line the current caret line (0 based)
     * @param column the current caret column (0 based)
     */
    void FindDefinition(const wxFileName& filename, size_t line, size_t column);
};

#endif // CLLANGUAGESERVER_H
