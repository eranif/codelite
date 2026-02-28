#include "clFileSystemWorkspaceConfig.hpp"

#include "Debugger/debuggermanager.h"
#include "ICompilerLocator.h"
#include "build_settings_config.h"
#include "compiler_command_line_parser.h"
#include "dirsaver.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "macromanager.h"
#include "macros.h"
#include "procutils.h"

#include <wx/tokenzr.h>

#define DEFAULT_FILE_EXTENSIONS                                                                                        \
    "*.cpp;*.c;*.txt;*.json;*.hpp;*.cc;*.cxx;*.xml;*.h;*.wxcp;*.py;*.php;*.rb;*.html;*.js;*.ts;*.rs;*.iss;*.md;*.bat;" \
    "*.sh;*.cmake"
#define WORKSPACE_TYPE "File System Workspace"
#define DEFAULT_EXCLUDE_FILE_PATTERN "*.o;*.pyc;*.obj;*.workspace;*.o.d;*.exe;*.dll;*.project"

wxDEFINE_EVENT(wxEVT_FSW_SETTINGS_SAVED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_FSW_SETTINGS_LOADED, clCommandEvent);
clFileSystemWorkspaceConfig::clFileSystemWorkspaceConfig()
    : m_fileExtensions(DEFAULT_FILE_EXTENSIONS)
    , m_excludeFilesPattern(DEFAULT_EXCLUDE_FILE_PATTERN)
{
    m_buildTargets.insert({ "build", "" });
    m_buildTargets.insert({ "clean", "" });
    m_debugger = DebuggerMgr::Get().GetActiveDebuggerName();
    CompilerPtr compiler = BuildSettingsConfigST::Get()->GetDefaultCompiler(COMPILER_DEFAULT_FAMILY);
    if (compiler) {
        m_compiler = compiler->GetName();
    }
}

///===-------------------------------------
/// A single workspace configuration
///===-------------------------------------
std::pair<JSONItem, JSONItem> clFileSystemWorkspaceConfig::ToJSON() const
{
    // we store items in two locations:
    // local & shared
    // the local shared is only accessible to the current machine
    // while the shared can be stored in git or any other source control
    JSONItem shared = JSONItem::createObject();
    shared.addProperty("name", GetName());
    JSONItem arrTargets = shared.AddArray("targets");

    for (const auto& vt : m_buildTargets) {
        JSONItem target = JSONItem::createArray();
        arrTargets.arrayAppend(target);
        target.arrayAppend(vt.first);
        target.arrayAppend(vt.second);
    }

    shared.addProperty("file_extensions", m_fileExtensions);
    shared.addProperty("excludeFilesPattern", m_excludeFilesPattern);
    shared.addProperty("excludePaths", m_excludePaths);
    shared.addProperty("debugger", m_debugger);

    // Local items
    JSONItem local = JSONItem::createObject();
    local.addProperty("name", GetName());
    local.addProperty("flags", m_flags);
    local.addProperty("compile_flags", m_compileFlags);
    local.addProperty("executable", m_executable);
    local.addProperty("arguments", m_args);
    local.addProperty("environment", m_environment);
    local.addProperty("compiler", m_compiler);
    local.addProperty("remoteFolder", m_remoteFolder);
    local.addProperty("remoteAccount", m_remoteAccount);
    local.addProperty("workingDirectory", m_workingDirectory);
    local.addProperty("debuggerPath", m_debuggerPath);
    local.addProperty("debuggerCommands", m_debuggerCommands);
    
    local.addProperty("debuggerRemoteEnabled",  m_debuggerRemoteEnabled);
    local.addProperty("debuggerRemoteExtended", m_debuggerRemoteExtended);
    local.addProperty("debuggerRemoteHost",     m_debuggerRemoteHost);
    local.addProperty("debuggerRemotePort",     m_debuggerRemotePort);
    local.addProperty("debuggerRemoteCommands", m_debuggerRemoteCommands);
    
    local.addProperty("last_executables", m_lastExecutables);
    return { shared, local };
}

void clFileSystemWorkspaceConfig::FromSharedJSON(const JSONItem& json)
{
    m_name = json.namedObject("name").toString();
    JSONItem arrTargets = json.namedObject("targets");
    int nCount = arrTargets.arraySize();
    m_buildTargets.clear();
    for (int i = 0; i < nCount; ++i) {
        JSONItem target = arrTargets.arrayItem(i);
        if (target.arraySize() != 2) {
            continue;
        }
        m_buildTargets.insert({ target[0].toString(), target[1].toString() });
    }
    m_fileExtensions = json.namedObject("file_extensions").toString(m_fileExtensions);
    m_excludeFilesPattern = json.namedObject("excludeFilesPattern").toString(m_excludeFilesPattern);
    m_excludePaths = json.namedObject("excludePaths").toString(m_excludePaths);
    m_debugger = json.namedObject("debugger").toString(m_debugger);
}

void clFileSystemWorkspaceConfig::FromLocalJSON(const JSONItem& json)
{
    m_name = json["name"].toString();
    m_flags = json["flags"].toSize_t(m_flags);
    m_compileFlags = json["compile_flags"].toArrayString();
    m_executable = json["executable"].toString();
    wxArrayString last_execs = json["last_executables"].toArrayString();

    // calling SetLastExecutables ensures that we don't add empty paths
    SetLastExecutables(last_execs);

    m_args = json["arguments"].toString();
    m_environment = json["environment"].toString();
    m_compiler = json["compiler"].toString(m_compiler);
    m_remoteFolder = json["remoteFolder"].toString();
    m_remoteAccount = json["remoteAccount"].toString();
    m_workingDirectory = json["workingDirectory"].toString();
    
    m_debuggerPath              = json["debuggerPath"].toString();
    m_debuggerCommands          = json["debuggerCommands"].toString();
    
    m_debuggerRemoteEnabled     = json["debuggerRemoteEnabled"].toBool();
    m_debuggerRemoteExtended    = json["debuggerRemoteExtended"].toBool(true);
    m_debuggerRemoteHost        = json["debuggerRemoteHost"].toString();
    m_debuggerRemotePort        = json["debuggerRemotePort"].toString();
    m_debuggerRemoteCommands    = json["debuggerRemoteCommands"].toString();
}

wxString clFileSystemWorkspaceConfig::GetCompileFlagsAsString() const
{
    wxString s;
    for (const wxString& l : m_compileFlags) {
        if (!l.IsEmpty()) {
            s << l << "\n";
        }
    }
    return s.Trim();
}

clFileSystemWorkspaceConfig::Ptr_t clFileSystemWorkspaceConfig::Clone() const
{
    return clFileSystemWorkspaceConfig::Ptr_t(new clFileSystemWorkspaceConfig(*this));
}

static wxArrayString GetExtraFlags(CompilerPtr compiler)
{
    if (compiler->HasMetadata()) {
        auto md = compiler->GetMetadata();
        if (!md.GetTarget().IsEmpty()) {
            return {wxString("-target"), md.GetTarget()};
        }
    }
    return {};
}

wxArrayString clFileSystemWorkspaceConfig::GetCompilerOptions(clBacktickCache::ptr_t backticks) const
{
    wxArrayString searchPaths;
    wxUnusedVar(backticks);
    // Add the compiler paths
    CompilerPtr compiler = BuildSettingsConfigST::Get()->GetCompiler(GetCompiler());
    if (compiler) {
        wxArrayString compilerPaths = compiler->GetDefaultIncludePaths();
        if (!compiler->GetGlobalIncludePath().IsEmpty()) {
            wxArrayString globalIncludePaths =
                ::wxStringTokenize(compiler->GetGlobalIncludePath(), ";", wxTOKEN_STRTOK);
            compilerPaths.insert(compilerPaths.end(), globalIncludePaths.begin(), globalIncludePaths.end());
        }
        for (wxString& compilerPath : compilerPaths) {
            compilerPath.Prepend("-I");
        }
        searchPaths.insert(searchPaths.end(), compilerPaths.begin(), compilerPaths.end());

        auto extraFlags = GetExtraFlags(compiler);
        if (!extraFlags.empty()) {
            searchPaths.insert(searchPaths.end(), extraFlags.begin(), extraFlags.end());
        }
    }
    return searchPaths;
}

wxArrayString clFileSystemWorkspaceConfig::ExpandUserCompletionFlags(const wxString& workingDirectory,
                                                                     clBacktickCache::ptr_t backticks,
                                                                     bool withPrefix) const
{
    wxArrayString searchPaths;
    for (const auto& line : m_compileFlags) {
        // we support up to one backtick in a line
        wxString backtick = line.AfterFirst('`');
        backtick = backtick.BeforeLast('`');

        wxString prefix;
        wxString suffix;
        if (line.Index('`') != wxString::npos) {
            prefix = line.BeforeFirst('`');
            suffix = line.AfterLast('`');
        } else {
            prefix = line;
        }
        wxString backtick_expanded;
        if (!backtick.empty()) {
            if (backticks && backticks->HasCommand(backtick)) {
                backtick_expanded = backticks->GetExpanded(backtick);
            } else {
                // we got backtick, expand it
                DirSaver ds;

                ::wxSetWorkingDirectory(workingDirectory);
                // Now run the command
                clDEBUG() << "Working directory is set to:" << workingDirectory << clEndl;
                clDEBUG() << "Running command:" << backtick << clEndl;
                backtick_expanded = ProcUtils::SafeExecuteCommand(backtick);
                backtick_expanded.Trim().Trim(false);
                // keep the result for future lookups
                if (backticks) {
                    backticks->SetCommand(backtick, backtick_expanded);
                    backticks->Save();
                }
                clDEBUG() << "Output:" << backtick_expanded << clEndl;
            }
        }
        wxString line_expanded = prefix + backtick_expanded + suffix;

        // Incase we got backticks, we need to apply the environment
        CompilerCommandLineParser cclp(line_expanded, workingDirectory);

        // Get the include paths (-I)
        if (withPrefix) {
            searchPaths.insert(searchPaths.end(), cclp.GetIncludesWithPrefix().begin(),
                               cclp.GetIncludesWithPrefix().end());
        } else {
            searchPaths.insert(searchPaths.end(), cclp.GetIncludes().begin(), cclp.GetIncludes().end());
        }
        // Get the macros (-D)
        searchPaths.insert(searchPaths.end(), cclp.GetMacrosWithPrefix().begin(), cclp.GetMacrosWithPrefix().end());
        if (!cclp.GetStandard().empty()) {
            // -std=NNN
            searchPaths.push_back(cclp.GetStandardWithPrefix());
        }
        // Other options
        searchPaths.insert(searchPaths.end(), cclp.GetOtherOptions().begin(), cclp.GetOtherOptions().end());
    }

    // expand any macro
    for (auto& path : searchPaths) {
        path = MacroManager::Instance()->Expand(path, nullptr, "", "");
    }
    return searchPaths;
}

void clFileSystemWorkspaceConfig::SetLastExecutables(const wxArrayString& lastExecutables)
{
    m_lastExecutables.clear();
    m_lastExecutables.reserve(lastExecutables.size());
    wxStringSet_t unique;
    for (auto path : lastExecutables) {
        path.Trim().Trim(false);
        if (path.empty()) {
            continue;
        }

        // Keep unique entries only
        if (unique.insert(path).second) {
            m_lastExecutables.Add(path);
        }
    }
}

const wxArrayString& clFileSystemWorkspaceConfig::GetLastExecutables() const { return m_lastExecutables; }

///===-------------------------------------
/// Workspace settings
///===-------------------------------------
clFileSystemWorkspaceSettings::clFileSystemWorkspaceSettings()
{
    Clear(); // Create with the default "Debug" target
}

void clFileSystemWorkspaceSettings::ToJSON(JSONItem& shared, JSONItem& local) const
{
    // add the shared items
    shared.addProperty("workspace_type", WORKSPACE_TYPE);
    shared.addProperty("name", m_name);

    // Add the shared items
    {
        JSONItem configs = shared.AddArray("configs");
        for (const auto& config : m_configsMap) {
            configs.arrayAppend(config.second->ToJSON().first);
        }
    }

    // add the local items
    {
        local.addProperty("selected_config", m_selectedConfig);
        JSONItem configs = local.AddArray("configs");
        for (const auto& config : m_configsMap) {
            configs.arrayAppend(config.second->ToJSON().second);
        }
    }
}

void clFileSystemWorkspaceSettings::FromJSON(const JSONItem& shared, const JSONItem& local)
{
    // first load the settings from the shared workspace config
    m_workspaceType = shared.namedObject("workspace_type").toString(WORKSPACE_TYPE);
    m_name = shared.namedObject("name").toString();
    // The "selected config" is kept in the local workspace
    m_selectedConfig = local.namedObject("selected_config").toString();
    // load the configurations
    JSONItem sharedConfigs = shared.namedObject("configs");
    JSONItem localConfigs = local.namedObject("configs");
    if (sharedConfigs.arraySize() != localConfigs.arraySize()) {
        clSYSTEM() << "Notice: File System Workspace: local config and shared configs do not match!" << endl;
    }

    // Loop over the local configs and keep them in a map
    // this is because the number of configuration entries in
    // the local workspace does not match to the shared one
    // this can happen when loading the workspace on different
    // machines (when the shared version is kept in SCM)
    std::unordered_map<wxString, JSONItem> localConfigsMap;

    int localCount = localConfigs.arraySize();
    for (int i = 0; i < localCount; ++i) {
        auto c = localConfigs[i];
        localConfigsMap.insert({ c["name"].toString(), c });
    }

    int nCount = sharedConfigs.arraySize();
    wxString firstConfig;
    bool selectedConfigFound = false;
    m_configsMap.clear();
    for (int i = 0; i < nCount; ++i) {
        clFileSystemWorkspaceConfig::Ptr_t conf(new clFileSystemWorkspaceConfig);
        conf->FromSharedJSON(sharedConfigs.arrayItem(i));
        auto iter = localConfigsMap.find(conf->GetName());
        if (iter != localConfigsMap.end()) {
            conf->FromLocalJSON(iter->second);
        }

        if (firstConfig.empty()) {
            firstConfig = conf->GetName();
        }
        if (!selectedConfigFound && (conf->GetName() == m_selectedConfig)) {
            selectedConfigFound = true;
        }
        m_configsMap.insert({ conf->GetName(), conf });
    }

    if (!selectedConfigFound && !firstConfig.IsEmpty()) {
        m_selectedConfig = firstConfig;
    }
}

bool clFileSystemWorkspaceSettings::Save(const wxFileName& filename, const wxFileName& localSettings)
{
    // store the
    wxFileName localWorkspace;

    if (localSettings.IsOk()) {
        localWorkspace = localSettings;
    } else {
        localWorkspace = filename;
        localWorkspace.AppendDir(".codelite");
    }
    localWorkspace.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    JSON root_local(JsonType::Object);
    JSON root_shared(JsonType::Object);

    JSONItem itemLocal = root_local.toElement();
    JSONItem itemShared = root_shared.toElement();
    ToJSON(itemShared, itemLocal);
    root_shared.save(filename);
    root_local.save(localWorkspace);

    clCommandEvent eventSaved(wxEVT_FSW_SETTINGS_SAVED);
    EventNotifier::Get()->AddPendingEvent(eventSaved);
    return true;
}

bool clFileSystemWorkspaceSettings::Load(const wxFileName& filename, const wxFileName& localSettings)
{
    wxFileName localWorkspace;
    if (localSettings.IsOk()) {
        localWorkspace = localSettings;
    } else {
        localWorkspace = filename;
        localWorkspace.AppendDir(".codelite");
    };

    JSON root_shared(filename);
    if (!root_shared.isOk()) {
        clWARNING() << "Invalid File System Workspace file:" << filename << endl;
        return false;
    }
    JSON root_local(localWorkspace);
    if (!root_local.isOk()) {
        // old version
        clDEBUG() << "clFileSystemWorkspaceSettings: no local file found. Loading from shared file" << clEndl;
        auto iShared = root_shared.toElement();
        FromJSON(iShared, iShared);
    } else {
        clDEBUG() << "clFileSystemWorkspaceSettings: loading settings from:" << filename << "and" << localWorkspace
                  << clEndl;
        auto iShared = root_shared.toElement();
        auto iLocal = root_local.toElement();
        FromJSON(iShared, iLocal);
    }
    clCommandEvent eventSaved(wxEVT_FSW_SETTINGS_LOADED);
    EventNotifier::Get()->AddPendingEvent(eventSaved);
    return true;
}

bool clFileSystemWorkspaceSettings::AddConfig(const wxString& name, const wxString& copyFrom)
{
    if (m_configsMap.count(name)) {
        // already exists
        clWARNING() << "Can't add new configuration: " << name << ". Already exists" << endl;
        return false;
    }

    clFileSystemWorkspaceConfig::Ptr_t conf;
    if (!copyFrom.IsEmpty() && GetConfig(copyFrom)) {
        // clone the config
        conf = GetConfig(copyFrom)->Clone();
    } else {
        // create an empty one
        conf.reset(new clFileSystemWorkspaceConfig());
    }
    conf->SetName(name);
    m_configsMap.insert({ name, conf });
    if (m_configsMap.size() == 1) {
        m_selectedConfig = conf->GetName();
    }
    return true;
}

bool clFileSystemWorkspaceSettings::DeleteConfig(const wxString& name)
{
    if (m_configsMap.count(name) == 0) {
        return false;
    }
    m_configsMap.erase(name);
    if (m_selectedConfig == name) {
        m_selectedConfig.clear();
        if (!m_configsMap.empty()) {
            m_selectedConfig = m_configsMap.begin()->second->GetName();
        }
    }
    return true;
}

clFileSystemWorkspaceConfig::Ptr_t clFileSystemWorkspaceSettings::GetSelectedConfig() const
{
    // sanity
    if (m_configsMap.empty()) {
        return clFileSystemWorkspaceConfig::Ptr_t(nullptr);
    }
    if (m_selectedConfig.empty() || (m_configsMap.count(m_selectedConfig) == 0)) {
        return clFileSystemWorkspaceConfig::Ptr_t(nullptr);
    }
    return m_configsMap.find(m_selectedConfig)->second;
}

clFileSystemWorkspaceConfig::Ptr_t clFileSystemWorkspaceSettings::GetConfig(const wxString& name) const
{
    if (name.empty()) {
        return GetSelectedConfig();
    }
    if (m_configsMap.count(name) == 0) {
        return clFileSystemWorkspaceConfig::Ptr_t(nullptr);
    }
    return m_configsMap.find(name)->second;
}

void clFileSystemWorkspaceSettings::Clear()
{
    m_configsMap.clear();
    m_selectedConfig.clear();
    m_name.clear();
    AddConfig("Debug");
}

bool clFileSystemWorkspaceSettings::SetSelectedConfig(const wxString& name)
{
    if (m_configsMap.count(name) == 0) {
        return false;
    }
    m_selectedConfig = name;
    return true;
}

bool clFileSystemWorkspaceSettings::IsOk(const wxFileName& filename)
{
    JSON root(filename);
    if (!root.isOk()) {
        return false;
    }
    return root.toElement().namedObject("workspace_type").toString() == WORKSPACE_TYPE;
}

wxArrayString clFileSystemWorkspaceSettings::GetConfigs() const
{
    wxArrayString arr;
    for (const auto& vt : m_configsMap) {
        arr.Add(vt.first);
    }
    return arr;
}
