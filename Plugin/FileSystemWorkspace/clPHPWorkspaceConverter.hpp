#ifndef CLPHPWORKSPACECONVERTER_HPP
#define CLPHPWORKSPACECONVERTER_HPP

#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/string.h>

/**
 * @brief convert an old PHP workspace into a File System Workspace
 *
 * The workspace folder becomes the root folder. The file types and exclude folders of all projects are merged.
 * Projects outside of the workspace folder are left out. The Xdebug settings of the active project and the
 * breakpoints go to .codelite/xdebug.json (the file of the XDebug debugger), and the SFTP settings become the
 * remote target of the configuration
 */
class WXDLLIMPEXP_SDK clPHPWorkspaceConverter
{
public:
    /**
     * @brief return true if the file is an old PHP workspace
     */
    static bool IsPHPWorkspace(const wxFileName& file);

    /**
     * @brief convert the PHP workspace in place. The old workspace file is renamed to a backup file first
     * @param warnings things that could not be converted
     * @param backup the backup file
     * @param error the reason for a failure
     * @return false when the conversion failed. The PHP workspace file is then unchanged
     */
    static bool Convert(const wxFileName& file, wxArrayString& warnings, wxFileName& backup, wxString& error);
};

#endif // CLPHPWORKSPACECONVERTER_HPP
