#ifndef CLLANGUAGESERVER_H
#define CLLANGUAGESERVER_H

#include "codelite_exports.h"
#include "asyncprocess.h"
#include "cl_command_event.h"
#include "clJSONRPC.h"

class WXDLLIMPEXP_CL clLanguageServer : public clJSONRPCSender
{
    IProcess* m_process = NULL;

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
     * @brief find definition of a word.
     * @param filename the filename
     * @param line the current caret line (0 based)
     * @param column the current caret column (0 based)
     */
    void FindDefinition(const wxFileName& filename, size_t line, size_t column);
};

#endif // CLLANGUAGESERVER_H
