#include "cmakehelpthread.h"
#include "CMake.h"
#include "globals.h"
#include "procutils.h"
#include <wx/ffile.h>


bool CMakeHelpThread::LoadList(const wxString& command, const wxString& type, CMake::HelpMap& list)
{
    // Get list
    wxArrayString names;
    ProcUtils::SafeExecuteCommand(command + " --help-" + type + "-list", names);

    if ( names.IsEmpty() )
        // something bad just happened
        return false;
        
    // Remove version
    names.RemoveAt(0);

    // Create temp file name
    // This is required because cmake is able export to HTML only into file
    wxString tmpFileName = wxFileName::CreateTempFileName("cmake_") + ".html";
    
    // Foreach names
    for (size_t i=0; i<names.GetCount(); ++i) {
        
        if ( TestDestroy() ) {
            return false;
        }
        
        // Export help
        wxArrayString dummy;
        wxString commandToRun;
        wxString commandName = names.Item(i);
        commandName.Trim().Trim(false);
        
        commandToRun << command 
                     << " --help-"
                     << type 
                     << " " 
                     << commandName 
                     << " " 
                     << tmpFileName;
        ProcUtils::SafeExecuteCommand(commandToRun, dummy);

        // Read help
        wxFFile fp(tmpFileName, "rb");
        if ( fp.IsOpened() ) {
            
            wxString html;
            fp.ReadAll( &html, wxConvUTF8 );
            fp.Close();
            
            // Store HTML page
            list.insert( std::make_pair(commandName, html) );
        }
    }
    return true;
}

CMakeHelpThread::CMakeHelpThread(CMake* cmake, const wxString &command)
    : wxThread(wxTHREAD_JOINABLE)
    , m_cmake(cmake)
    , m_command(command)
{
}

CMakeHelpThread::~CMakeHelpThread()
{
}

void* CMakeHelpThread::Entry()
{
    CMake::HelpMap* commands = new CMake::HelpMap;
    if ( !LoadList(m_command, "command", *commands) )
        return NULL;
        
    m_cmake->CallAfter( &CMake::OnCommandsHelpLoaded, commands );
        
    CMake::HelpMap* property = new CMake::HelpMap;
    if ( !LoadList(m_command, "property", *property) )
        return NULL;
        
    m_cmake->CallAfter( &CMake::OnPropertiesHelpLoaded, property );
    
    CMake::HelpMap* module = new CMake::HelpMap;
    if ( !LoadList(m_command, "module", *module) )
        return NULL;
    m_cmake->CallAfter( &CMake::OnModulesHelpLoaded, module );
    
    CMake::HelpMap* variable = new CMake::HelpMap;
    if ( !LoadList(m_command, "variable", *variable) )
        return NULL;
        
    m_cmake->CallAfter( &CMake::OnVariablesHelpLoaded, variable );
    
    // Notify that we are done
    m_cmake->CallAfter( &CMake::OnHelpLoadCompleted );
    return NULL;
}
