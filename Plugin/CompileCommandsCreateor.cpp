#include "CompileCommandsCreateor.h"
#include "event_notifier.h"
#include "workspace.h"

wxDEFINE_EVENT(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, clCommandEvent);

CompileCommandsCreateor::CompileCommandsCreateor(const wxFileName& path)
    : m_filename(path)
{
}

CompileCommandsCreateor::~CompileCommandsCreateor()
{
}

void CompileCommandsCreateor::Process(wxThread* thread)
{
    wxString errMsg;
    wxUnusedVar(thread);
    Workspace workspace;
    workspace.OpenReadOnly(m_filename.GetFullPath(), errMsg);
    
    JSONRoot json(cJSON_Array);
    JSONElement compile_commands = json.toElement();
    workspace.CreateCompileCommandsJSON(compile_commands);
    
    // Save the file
    wxFileName compileCommandsFile(m_filename.GetPath(), "compile_commands.json");
    json.save( compileCommandsFile );
    
    clCommandEvent eventCompileCommandsGenerated(wxEVT_COMPILE_COMMANDS_JSON_GENERATED);
    EventNotifier::Get()->AddPendingEvent( eventCompileCommandsGenerated );
}
