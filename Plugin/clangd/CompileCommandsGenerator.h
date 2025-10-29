#ifndef COMPILECOMMANDSGENERATOR_H
#define COMPILECOMMANDSGENERATOR_H

#include "AsyncProcess/asyncprocess.h"
#include "cl_command_event.h"
#include "codelite_exports.h"
#include "macros.h"

#include <memory>
#include <wx/event.h>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

class WXDLLIMPEXP_SDK CompileCommandsGenerator : public wxEvtHandler
{
    IProcess* m_process = nullptr;
    wxString m_capturedOutput;

protected:
    void OnProcessTerminated(clProcessEvent& event);
    void OnProcessOutput(clProcessEvent& event);

public:
    using Ptr_t = std::shared_ptr<CompileCommandsGenerator>;
    CompileCommandsGenerator();
    virtual ~CompileCommandsGenerator();

    void GenerateCompileCommands();
};

#endif // COMPILECOMMANDSGENERATOR_H
