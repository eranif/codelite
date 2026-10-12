#ifndef XDEBUGSETTINGS_H
#define XDEBUGSETTINGS_H

#include "XDebugBreakpoint.h"
#include "macros.h"
#include "php_project_settings_data.h"

#include <wx/filename.h>
#include <wx/string.h>

/**
 * @brief the Xdebug settings of the open workspace
 * For a PHP workspace, these are the settings of the active project. For any other (local) workspace, they are
 * stored in <workspace folder>/.codelite/xdebug.json, together with the Xdebug breakpoints
 */
class XDebugSettings
{
    PHPProjectSettingsData m_data;
    wxStringMap_t m_fileMapping;
    wxString m_projectName;
    wxFileName m_file;
    bool m_ok = false;

public:
    /**
     * @brief the debugger name that a workspace uses to select Xdebug
     */
    static const wxString DEBUGGER_NAME;

    /**
     * @brief return true if Xdebug is the debugger of the open workspace
     */
    static bool IsActive();

    /**
     * @brief return the Xdebug settings file of the open workspace. Not valid for a PHP workspace or when no local
     * workspace is open
     */
    static wxFileName GetSettingsFile();

    static XDebugBreakpoint::List_t LoadBreakpoints(const wxFileName& file);
    /**
     * @brief store the breakpoints in the settings file. The file is not created for an empty list
     */
    static void SaveBreakpoints(const wxFileName& file, const XDebugBreakpoint::List_t& breakpoints);

    /**
     * @brief load the settings of the open workspace
     * @return false if there is no workspace or no active PHP project
     */
    bool Load();
    void Save();
    bool IsOk() const { return m_ok; }

    PHPProjectSettingsData& GetData() { return m_data; }
    const PHPProjectSettingsData& GetData() const { return m_data; }

    /**
     * @brief return the file mapping (local folder -> remote folder). This includes the remote folder of the
     * workspace, if one is used
     */
    const wxStringMap_t& GetFileMapping() const { return m_fileMapping; }
};

#endif // XDEBUGSETTINGS_H
