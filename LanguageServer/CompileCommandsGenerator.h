#ifndef COMPILECOMMANDSGENERATOR_H
#define COMPILECOMMANDSGENERATOR_H

#include <wx/event.h>
#include "asyncprocess.h"
#include "cl_command_event.h"
#include <wx/filename.h>

class CompileCommandsGenerator : public wxEvtHandler
{
    IProcess* m_process = nullptr;
    wxFileName m_outputFile;

protected:
    void OnProcessOutput(clProcessEvent& event);
    void OnProcessTeraminated(clProcessEvent& event);

public:
    CompileCommandsGenerator();
    virtual ~CompileCommandsGenerator();

    void GenerateCompileCommands();
};

#endif // COMPILECOMMANDSGENERATOR_H
