#include "ICompilerLocator.h"
#include "build_settings_config.h"
#include "clFileSystemWorkspace.hpp"
#include "clFileSystemWorkspaceConfig.hpp"
#include "compiler_command_line_parser.h"
#include "ctags_manager.h"
#include "debuggermanager.h"
#include "dirsaver.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "macromanager.h"
#include "procutils.h"

#define DEFAULT_FILE_EXTENSIONS "*.cpp;*.c;*.txt;*.json;*.hpp;*.cc;*.cxx;*.xml;*.h;*.wxcp;*.py;*.php;*.rb;*.html;*.js"
#define WORKSPACE_TYPE "File System Workspace"
#define DEFAULT_EXCLUDE_FILE_PATTERN "*.o;*.pyc;*.obj;*.workspace;*.o.d;*.exe;*.dll;*.project"

clFileSystemWorkspaceConfig::clFileSystemWorkspaceConfig()
    : m_fileExtensions(DEFAULT_FILE_EXTENSIONS)
    , m_excludeFilesPattern(DEFAULT_EXCLUDE_FILE_PATTERN)
{
    m_buildTargets.insert({ "build", "" });
    m_buildTargets.insert({ "clean", "" });
    m_debugger = DebuggerMgr::Get().GetActiveDebuggerName();
    CompilerPtr compiler = BuildSettingsConfigST::Get()->GetDefaultCompiler(COMPILER_DEFAULT_FAMILY);
    if(compiler) {
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

    for(const auto& vt : m_buildTargets) {
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
    return { shared, local };
}

void clFileSystemWorkspaceConfig::FromSharedJSON(const JSONItem& json)
{
    m_name = json.namedObject("name").toString();
    JSONItem arrTargets = json.namedObject("targets");
    int nCount = arrTargets.arraySize();
    m_buildTargets.clear();
    for(int i = 0; i < nCount; ++i) {
        JSONItem target = arrTargets.arrayItem(i);
        if(target.arraySize() != 2) {
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
    m_args = json["arguments"].toString();
    m_environment = json["environment"].toString();
    m_compiler = json["compiler"].toString(m_compiler);
    m_remoteFolder = json["remoteFolder"].toString();
    m_remoteAccount = json["remoteAccount"].toString();
    m_workingDirectory = json["workingDirectory"].toString();
}

void clFileSystemWorkspaceConfig::FromJSONOld(const JSONItem& json)
{
    FromSharedJSON(json);
    FromLocalJSON(json);
}

wxArrayString clFileSystemWorkspaceConfig::GetSearchPaths(const wxFileName& workspaceFile) const
{
    // Update the parser search paths (the default compiler paths)
    auto backticks_cache = clFileSystemWorkspace::Get().GetBackticksCache();
    wxArrayString searchPaths = TagsManagerST::Get()->GetCtagsOptions().GetParserSearchPaths();
    wxArrayString userPaths = ExpandUserCompletionFlags(workspaceFile.GetPath(), backticks_cache, false);

    // Append the lists
    searchPaths.insert(searchPaths.end(), userPaths.begin(), userPaths.end());

    wxArrayString uniquePaths;
    std::unordered_set<wxString> S;
    for(const wxString& path : searchPaths) {
        wxFileName fn(path, "");
        wxString fixedPath = fn.GetPath();
        if(S.count(fixedPath) == 0) {
            S.insert(fixedPath);
            uniquePaths.Add(fixedPath);
        }
    }
    return uniquePaths;
}

wxString clFileSystemWorkspaceConfig::GetCompileFlagsAsString() const
{
    wxString s;
    for(const wxString& l : m_compileFlags) {
        if(!l.IsEmpty()) {
            s << l << "\n";
        }
    }
    return s.Trim();
}

void clFileSystemWorkspaceConfig::SetCompileFlags(const wxString& compileFlags)
{
    m_compileFlags = ::wxStringTokenize(compileFlags, "\r\n", wxTOKEN_STRTOK);
}

clFileSystemWorkspaceConfig::Ptr_t clFileSystemWorkspaceConfig::Clone() const
{
    return clFileSystemWorkspaceConfig::Ptr_t(new clFileSystemWorkspaceConfig(*this));
}

static wxArrayString GetExtraFlags(CompilerPtr compiler)
{
    wxArrayString flags;
#ifdef __WXMSW__
    if(compiler->GetCompilerFamily() == COMPILER_FAMILY_MINGW) {
        flags.Add("-target");
        if(compiler->Is64BitCompiler()) {
            flags.Add("x86_64-pc-windows-gnu");
        } else {
            flags.Add("i686-pc-windows-gnu");
        }
    }
#endif
    return flags;
}

wxArrayString clFileSystemWorkspaceConfig::GetCompilerOptions(clBacktickCache::ptr_t backticks) const
{
    wxArrayString searchPaths;
    wxUnusedVar(backticks);
    // Add the compiler paths
    CompilerPtr compiler = BuildSettingsConfigST::Get()->GetCompiler(GetCompiler());
    if(compiler) {
        wxArrayString compilerPaths = compiler->GetDefaultIncludePaths();
        for(wxString& compilerPath : compilerPaths) {
            compilerPath.Prepend("-I");
        }
        searchPaths.insert(searchPaths.end(), compilerPaths.begin(), compilerPaths.end());

        auto extraFlags = GetExtraFlags(compiler);
        if(!extraFlags.empty()) {
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
    for(const auto& line : m_compileFlags) {
        // we support up to one backtick in a line
        wxString backtick = line.AfterFirst('`');
        backtick = backtick.BeforeLast('`');

        wxString prefix;
        wxString suffix;
        if(line.Index('`') != wxString::npos) {
            prefix = line.BeforeFirst('`');
            suffix = line.AfterLast('`');
        } else {
            prefix = line;
        }
        wxString backtick_expanded;
        if(!backtick.empty()) {
            if(backticks && backticks->HasCommand(backtick)) {
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
                if(backticks) {
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
        if(withPrefix) {
            searchPaths.insert(searchPaths.end(), cclp.GetIncludesWithPrefix().begin(),
                               cclp.GetIncludesWithPrefix().end());
        } else {
            searchPaths.insert(searchPaths.end(), cclp.GetIncludes().begin(), cclp.GetIncludes().end());
        }
        // Get the macros (-D)
        searchPaths.insert(searchPaths.end(), cclp.GetMacrosWithPrefix().begin(), cclp.GetMacrosWithPrefix().end());
        if(!cclp.GetStandard().empty()) {
            // -std=NNN
            searchPaths.push_back(cclp.GetStandardWithPrefix());
        }
    }

    // expand any macro
    for(auto& path : searchPaths) {
        path = MacroManager::Instance()->Expand(path, nullptr, "", "");
    }
    return searchPaths;
}

wxArrayString clFileSystemWorkspaceConfig::GetWorkspaceIncludes(bool withPrefix) const
{
    auto all_files = clFileSystemWorkspace::Get().GetFiles();

    // collect list of paths from the workspace
    wxArrayString workspaceDirsArr;
    wxStringSet_t workspaceDirsSet;
    for(const auto& file : all_files) {
        if(!FileExtManager::IsCxxFile(file.GetFullName())) {
            continue;
        }
        wxString path = file.GetPath();
        if(path.Contains("/CMakeFiles")) {
            // CMake internal folder, ignore it
            continue;
        }
        if(workspaceDirsSet.count(path) == 0) {
            workspaceDirsSet.insert(path);

            wxString fixedPath;
            if(withPrefix) {
                fixedPath << "-I";
            }
            fixedPath << path;
            workspaceDirsArr.Add(fixedPath);
        }
    }
    return workspaceDirsArr;
}

///===-------------------------------------
/// Workspace settings
///===-------------------------------------
clFileSystemWorkspaceSettings::clFileSystemWorkspaceSettings()
{
    Clear(); // Create with the default "Debug" target
}

clFileSystemWorkspaceSettings::~clFileSystemWorkspaceSettings() {}

void clFileSystemWorkspaceSettings::ToJSON(JSONItem& shared, JSONItem& local) const
{
    // add the shared items
    shared.addProperty("workspace_type", WORKSPACE_TYPE);
    shared.addProperty("name", m_name);

    // Add the shared items
    {
        JSONItem configs = shared.AddArray("configs");
        for(const auto& config : m_configsMap) {
            configs.arrayAppend(config.second->ToJSON().first);
        }
    }

    // add the local items
    {
        local.addProperty("selected_config", m_selectedConfig);
        JSONItem configs = local.AddArray("configs");
        for(const auto& config : m_configsMap) {
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
    JSONItem localConfigs = shared.namedObject("configs");
    JSONItem sharedConfigs = local.namedObject("configs");
    if(sharedConfigs.arraySize() != localConfigs.arraySize()) {
        clWARNING() << "Local config and shared configs do not match!" << clEndl;
        return;
    }

    int nCount = localConfigs.arraySize();
    wxString firstConfig;
    bool selectedConfigFound = false;
    m_configsMap.clear();
    for(int i = 0; i < nCount; ++i) {
        clFileSystemWorkspaceConfig::Ptr_t conf(new clFileSystemWorkspaceConfig);
        conf->FromSharedJSON(localConfigs.arrayItem(i));
        conf->FromLocalJSON(sharedConfigs.arrayItem(i));
        if(firstConfig.empty()) {
            firstConfig = conf->GetName();
        }
        if(!selectedConfigFound && (conf->GetName() == m_selectedConfig)) {
            selectedConfigFound = true;
        }
        m_configsMap.insert({ conf->GetName(), conf });
    }

    if(!selectedConfigFound && !firstConfig.IsEmpty()) {
        m_selectedConfig = firstConfig;
    }
}

bool clFileSystemWorkspaceSettings::Save(const wxFileName& filename)
{
    // store the
    wxFileName localWorkspace = filename;
    localWorkspace.AppendDir(".codelite");
    localWorkspace.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    JSON root_local(cJSON_Object);
    JSON root_shared(cJSON_Object);

    JSONItem itemLocal = root_local.toElement();
    JSONItem itemShared = root_shared.toElement();
    ToJSON(itemShared, itemLocal);
    root_shared.save(filename);
    root_local.save(localWorkspace);
    return true;
}

bool clFileSystemWorkspaceSettings::Load(const wxFileName& filename)
{
    wxFileName localWorkspace = filename;
    localWorkspace.AppendDir(".codelite");
    JSON root_shared(filename);
    JSON root_local(localWorkspace);
    if(!root_local.isOk()) {
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
    return true;
}

bool clFileSystemWorkspaceSettings::AddConfig(const wxString& name, const wxString& copyfrom)
{
    if(m_configsMap.count(name)) {
        // already exists
        clWARNING() << "Can't add new configurtion:" << name << ". Already exists" << endl;
        return false;
    }

    clFileSystemWorkspaceConfig::Ptr_t conf;
    if(!copyfrom.IsEmpty() && GetConfig(copyfrom)) {
        // clone the config
        conf = GetConfig(copyfrom)->Clone();
    } else {
        // create an empty one
        conf.reset(new clFileSystemWorkspaceConfig());
    }
    conf->SetName(name);
    m_configsMap.insert({ name, conf });
    if(m_configsMap.size() == 1) {
        m_selectedConfig = conf->GetName();
    }
    return true;
}

bool clFileSystemWorkspaceSettings::DeleteConfig(const wxString& name)
{
    if(m_configsMap.count(name) == 0) {
        return false;
    }
    m_configsMap.erase(name);
    if(m_selectedConfig == name) {
        m_selectedConfig.clear();
        if(!m_configsMap.empty()) {
            m_selectedConfig = m_configsMap.begin()->second->GetName();
        }
    }
    return true;
}

clFileSystemWorkspaceConfig::Ptr_t clFileSystemWorkspaceSettings::GetSelectedConfig() const
{
    // sanity
    if(m_configsMap.empty()) {
        return clFileSystemWorkspaceConfig::Ptr_t(nullptr);
    }
    if(m_selectedConfig.empty() || (m_configsMap.count(m_selectedConfig) == 0)) {
        return clFileSystemWorkspaceConfig::Ptr_t(nullptr);
    }
    return m_configsMap.find(m_selectedConfig)->second;
}

clFileSystemWorkspaceConfig::Ptr_t clFileSystemWorkspaceSettings::GetConfig(const wxString& name) const
{
    if(name.empty()) {
        return GetSelectedConfig();
    }
    if(m_configsMap.count(name) == 0) {
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
    if(m_configsMap.count(name) == 0) {
        return false;
    }
    m_selectedConfig = name;
    return true;
}

bool clFileSystemWorkspaceSettings::IsOk(const wxFileName& filename)
{
    JSON root(filename);
    if(!root.isOk()) {
        return false;
    }
    return root.toElement().namedObject("workspace_type").toString() == WORKSPACE_TYPE;
}

wxArrayString clFileSystemWorkspaceSettings::GetConfigs() const
{
    wxArrayString arr;
    for(const auto& vt : m_configsMap) {
        arr.Add(vt.first);
    }
    return arr;
}
