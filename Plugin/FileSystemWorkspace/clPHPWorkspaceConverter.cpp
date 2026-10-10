#include "clPHPWorkspaceConverter.hpp"

#include "JSON.h"
#include "clFileSystemWorkspaceConfig.hpp"
#include "fileutils.h"
#include "globals.h"

#include <wx/filefn.h>
#include <wx/tokenzr.h>

namespace
{
/// The debugger name that the XDebug plugin registers
const wxString XDEBUG_DEBUGGER_NAME = "XDebug";

/// Add the entries of a ';' separated list that are not in the array yet
void AddEntries(const wxString& list, wxArrayString& entries)
{
    for (const wxString& entry : wxStringTokenize(list, ";", wxTOKEN_STRTOK)) {
        if (entries.Index(entry) == wxNOT_FOUND) {
            entries.Add(entry);
        }
    }
}

bool IsInsideFolder(const wxString& path, const wxString& folder)
{
    wxFileName relative(path, "");
    relative.MakeRelativeTo(folder);
    return relative.IsRelative() && (relative.GetDirCount() == 0 || relative.GetDirs()[0] != "..");
}

/// Read a file that may be missing, without an error message
wxString ReadFile(const wxFileName& file)
{
    wxString content;
    if (file.FileExists()) {
        FileUtils::ReadFileContent(file, content);
    }
    return content;
}

wxFileName GetPrivateFile(const wxFileName& workspaceFile, const wxString& fullname)
{
    wxFileName file(workspaceFile.GetPath(), fullname);
    file.AppendDir(".codelite");
    return file;
}
} // namespace

bool clPHPWorkspaceConverter::IsPHPWorkspace(const wxFileName& file)
{
    JSON root(ReadFile(file));
    if (!root.isOk() || !root.toElement().isObject()) {
        return false;
    }
    // The same test as the PHP plugin used
    JSONItem element = root.toElement();
    return element.namedObject("metadata").namedObject("type").toString() == "php" ||
           element.hasNamedObject("projects");
}

bool clPHPWorkspaceConverter::Convert(const wxFileName& file,
                                      wxArrayString& warnings,
                                      wxFileName& backup,
                                      wxString& error)
{
    JSON root(ReadFile(file));
    if (!root.isOk()) {
        error = _("Could not read the workspace file");
        return false;
    }

    const wxString workspaceFolder = file.GetPath();
    wxArrayString fileExtensions;
    wxArrayString excludeFolders;
    JSONItem xdebugSettings(nullptr);
    bool xdebugSettingsFromActiveProject = false;

    JSONItem projects = root.toElement().namedObject("projects");
    int count = projects.arraySize();
    for (int i = 0; i < count; ++i) {
        wxFileName projectFile(projects.arrayItem(i).toString());
        projectFile.MakeAbsolute(workspaceFolder);
        JSON project(ReadFile(projectFile));
        if (!project.isOk()) {
            warnings.Add(wxString::Format(_("Could not read the project file %s"), projectFile.GetFullPath()));
            continue;
        }

        JSONItem element = project.toElement();
        wxString name = element.namedObject("m_name").toString(projectFile.GetName());
        if (!IsInsideFolder(projectFile.GetPath(), workspaceFolder)) {
            warnings.Add(wxString::Format(
                _("Project %s is outside of the workspace folder and was left out: %s"), name, projectFile.GetPath()));
            continue;
        }

        AddEntries(element.namedObject("m_importFileSpec").toString(), fileExtensions);
        // The PHP project excludes folders by name, at any depth. A File System Workspace does the same for an
        // entry without a path separator
        AddEntries(element.namedObject("m_excludeFolders").toString(), excludeFolders);

        // Use the Xdebug settings of the active project, or else of the first project
        bool isActive = element.namedObject("m_isActive").toBool();
        if (!xdebugSettingsFromActiveProject && (isActive || !xdebugSettings.isOk())) {
            xdebugSettings = element.namedObject("settings");
            xdebugSettingsFromActiveProject = isActive;
        }
    }

    clFileSystemWorkspaceSettings settings;
    settings.SetName(file.GetName());
    auto config = settings.GetSelectedConfig();
    if (!config) {
        error = _("Could not create the workspace configuration");
        return false;
    }
    if (!fileExtensions.IsEmpty()) {
        config->SetFileExtensions(wxJoin(fileExtensions, ';'));
    }
    config->SetExcludePaths(wxJoin(excludeFolders, ';'));
    config->SetDebugger(XDEBUG_DEBUGGER_NAME);

    // The SFTP settings of the PHP workspace become the remote target
    JSON sftp(ReadFile(GetPrivateFile(file, "php-sftp.conf")));
    if (sftp.isOk()) {
        JSONItem element = sftp.toElement().namedObject("sftp");
        wxString account = element.namedObject("m_account").toString();
        wxString remoteFolder = element.namedObject("m_remoteFolder").toString();
        bool uploadEnabled = element.namedObject("m_remoteUploadEnabled").toBool();
        if (!account.IsEmpty() && !remoteFolder.IsEmpty()) {
            config->SetRemoteAccount(account);
            config->SetRemoteFolder(remoteFolder);
            config->SetRemoteEnabled(uploadEnabled);
            config->SetSyncOnSave(uploadEnabled);
        }
    }

    // Keep the old workspace file, and write the new one in its place, so the recent workspaces list still works
    backup = file;
    backup.SetFullName(file.GetFullName() + ".php-backup");
    for (int i = 1; backup.FileExists(); ++i) {
        backup.SetFullName(wxString::Format("%s.php-backup.%d", file.GetFullName(), i));
    }
    if (!wxRenameFile(file.GetFullPath(), backup.GetFullPath(), false)) {
        error = wxString::Format(_("Could not rename the workspace file to %s"), backup.GetFullPath());
        return false;
    }
    settings.Save(file);
    if (!file.FileExists()) {
        wxRenameFile(backup.GetFullPath(), file.GetFullPath(), false);
        error = _("Could not write the new workspace file");
        return false;
    }

    // The Xdebug settings and breakpoints, in the format of the XDebug plugin
    wxFileName xdebugFile = GetPrivateFile(file, "xdebug.json");
    JSON userWorkspace(ReadFile(GetPrivateFile(file, file.GetFullName() + "." + ::clGetUserName())));
    JSONItem breakpoints =
        userWorkspace.isOk() ? userWorkspace.toElement().namedObject("m_breakpoints") : JSONItem(nullptr);
    bool hasBreakpoints = breakpoints.isOk() && breakpoints.arraySize() > 0;
    if (xdebugFile.FileExists()) {
        warnings.Add(wxString::Format(
            _("The Xdebug settings were not changed, because %s already exists"), xdebugFile.GetFullPath()));
    } else if (xdebugSettings.isOk() || hasBreakpoints) {
        JSON xdebug(JsonType::Object);
        if (xdebugSettings.isOk()) {
            xdebug.toElement().addProperty("settings", xdebugSettings);
        }
        if (hasBreakpoints) {
            xdebug.toElement().addProperty("breakpoints", breakpoints);
        }
        xdebug.save(xdebugFile);
    }
    return true;
}
