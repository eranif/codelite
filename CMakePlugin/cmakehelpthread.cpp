#include "cmakehelpthread.h"
#include "CMake.h"
#include "globals.h"
#include "procutils.h"
#include <wx/ffile.h>

/**
 * @brief Loads help of type from command into list.
 *
 * @param command CMake command.
 * @param type    Help type.
 * @param list    Output variable.
 */
static void LoadList(const wxString& command, const wxString& type, CMake::HelpMap& list)
{
    // Get list
    wxArrayString names;
    ProcUtils::SafeExecuteCommand(command + " --help-" + type + "-list", names);

    if ( names.IsEmpty() )
        return;
        
    // Remove version
    names.RemoveAt(0);

    // Create temp file name
    // This is required because cmake is able export to HTML only into file
    wxString tmpFileName = wxFileName::CreateTempFileName("cmake_") + ".html";
    
    // Foreach names
    for (size_t i=0; i<names.GetCount(); ++i) {
        
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
    LoadList(m_command, "command", *commands);
    m_cmake->CallAfter( &CMake::OnCommandsHelpLoaded, commands );
    if ( TestDestroy() )
        return NULL;
        
    CMake::HelpMap* property = new CMake::HelpMap;
    LoadList(m_command, "property", *property);
    m_cmake->CallAfter( &CMake::OnPropertiesHelpLoaded, property );
    if ( TestDestroy() )
        return NULL;
    
    CMake::HelpMap* module = new CMake::HelpMap;
    LoadList(m_command, "module", *module);
    m_cmake->CallAfter( &CMake::OnModulesHelpLoaded, module );
    if ( TestDestroy() )
        return NULL;
    
    CMake::HelpMap* variable = new CMake::HelpMap;
    LoadList(m_command, "variable", *variable);
    m_cmake->CallAfter( &CMake::OnVariablesHelpLoaded, variable );
    
    // Notify that we are done
    m_cmake->CallAfter( &CMake::OnHelpLoadCompleted );
    return NULL;
}
