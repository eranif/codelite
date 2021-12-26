#ifndef COMPILECOMMANDSGENERATOR_H
#define COMPILECOMMANDSGENERATOR_H

#include "asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <macros.h>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/process.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

class WXDLLIMPEXP_SDK CompileCommandsGenerator : public wxEvtHandler
{
    IProcess* m_process = nullptr;
    wxString m_capturedOutput;

protected:
    void OnProcessTeraminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);

public:
    typedef wxSharedPtr<CompileCommandsGenerator> Ptr_t;
    CompileCommandsGenerator();
    virtual ~CompileCommandsGenerator();

    void GenerateCompileCommands();
};

#endif // COMPILECOMMANDSGENERATOR_H
