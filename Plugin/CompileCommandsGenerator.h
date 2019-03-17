#ifndef COMPILECOMMANDSGENERATOR_H
#define COMPILECOMMANDSGENERATOR_H

#include <wx/event.h>
#include "asyncprocess.h"
#include "cl_command_event.h"
#include <wx/filename.h>
#include <wx/process.h>
#include "codelite_exports.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

class WXDLLIMPEXP_SDK CompileCommandsGenerator : public wxEvtHandler
{
    wxProcess* m_process = nullptr;
    wxFileName m_outputFile;
    
protected:
    void OnProcessTeraminated(wxProcessEvent& event);

public:
    typedef wxSharedPtr<CompileCommandsGenerator> Ptr_t;
    CompileCommandsGenerator();
    virtual ~CompileCommandsGenerator();

    void GenerateCompileCommands();
};

#endif // COMPILECOMMANDSGENERATOR_H
