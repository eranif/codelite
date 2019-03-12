#ifndef COMPILECOMMANDSGENERATOR_H
#define COMPILECOMMANDSGENERATOR_H

#include <wx/event.h>
#include "asyncprocess.h"
#include "cl_command_event.h"
#include <wx/filename.h>
#include "ChildProcess.h"

class CompileCommandsGenerator : public wxEvtHandler
{
    ChildProcess::Ptr_t m_childProcess;
    wxFileName m_outputFile;

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTeraminated(clProcessEvent& event);

public:
    typedef wxSharedPtr<CompileCommandsGenerator> Ptr_t;
    CompileCommandsGenerator();
    virtual ~CompileCommandsGenerator();

    void GenerateCompileCommands();
};

#endif // COMPILECOMMANDSGENERATOR_H
