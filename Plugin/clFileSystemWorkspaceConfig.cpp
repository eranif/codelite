#include "ICompilerLocator.h"
#include "build_settings_config.h"
#include "clFileSystemWorkspaceConfig.hpp"
#include "compiler_command_line_parser.h"
#include "ctags_manager.h"
#include "debuggermanager.h"

#define DEFAULT_FILE_EXTENSIONS "*.cpp;*.c;*.txt;*.json;*.hpp;*.cc;*.cxx;*.xml;*.h;*.wxcp"
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
    if(compiler) { m_compiler = compiler->GetName(); }
}

///===-------------------------------------
/// A single workspace configuration
///===-------------------------------------
JSONItem clFileSystemWorkspaceConfig::ToJSON() const
{
    JSONItem item = JSONItem::createObject();
    item.addProperty("name", GetName());
    item.addProperty("flags", m_flags);
    item.addProperty("compile_flags", m_compileFlags);
    item.addProperty("file_extensions", m_fileExtensions);
    item.addProperty("executable", m_executable);
    item.addProperty("arguments", m_args);
    item.addProperty("environment", m_environment);
    item.addProperty("compiler", m_compiler);
    item.addProperty("remoteFolder", m_remoteFolder);
    item.addProperty("remoteAccount", m_remoteAccount);
    item.addProperty("debugger", m_debugger);
    item.addProperty("excludeFilesPattern", m_excludeFilesPattern);
    JSONItem arrTargets = JSONItem::createArray("targets");
    item.append(arrTargets);

    for(const auto& vt : m_buildTargets) {
        JSONItem target = JSONItem::createArray();
        target.arrayAppend(vt.first);
        target.arrayAppend(vt.second);
        arrTargets.arrayAppend(target);
    }
    return item;
}

void clFileSystemWorkspaceConfig::FromJSON(const JSONItem& json)
{
    m_name = json.namedObject("name").toString();
    m_flags = json.namedObject("flags").toSize_t(m_flags);
    m_compileFlags = json.namedObject("compile_flags").toArrayString();
    m_fileExtensions = json.namedObject("file_extensions").toString(m_fileExtensions);
    m_executable = json.namedObject("executable").toString();
    m_args = json.namedObject("arguments").toString();
    m_environment = json.namedObject("environment").toString();
    m_compiler = json.namedObject("compiler").toString(m_compiler);
    m_remoteFolder = json.namedObject("remoteFolder").toString();
    m_remoteAccount = json.namedObject("remoteAccount").toString();
    m_debugger = json.namedObject("debugger").toString(m_debugger);
    m_excludeFilesPattern = json.namedObject("excludeFilesPattern").toString(m_excludeFilesPattern);
    JSONItem arrTargets = json.namedObject("targets");
    int nCount = arrTargets.arraySize();
    m_buildTargets.clear();
    for(int i = 0; i < nCount; ++i) {
        JSONItem target = arrTargets.arrayItem(i);
        if(target.arraySize() != 2) { continue; }
        m_buildTargets.insert({ target.arrayItem(0).toString(), target.arrayItem(1).toString() });
    }
}

wxArrayString clFileSystemWorkspaceConfig::GetSearchPaths(const wxFileName& workspaceFile,
                                                          wxString& compile_flags_txt) const
{
    // Update the parser search paths (the default compiler paths)
    wxArrayString searchPaths = TagsManagerST::Get()->GetCtagsOptions().GetParserSearchPaths();

    // Read the user added paths
    wxString strCompileFlags;
    for(const wxString& l : m_compileFlags) {
        if(!l.IsEmpty()) { strCompileFlags << l << " "; }
    }
    strCompileFlags.Trim();

    // Incase we got backticks, we need to apply the environment
    CompilerCommandLineParser cclp(strCompileFlags, workspaceFile.GetPath());
    searchPaths.insert(searchPaths.end(), cclp.GetIncludes().begin(), cclp.GetIncludes().end());

    wxArrayString options;
    if(ShouldCreateCompileFlags()) {
        // Create the compile_flags.txt file content
        options.insert(options.end(), cclp.GetIncludesWithPrefix().begin(), cclp.GetIncludesWithPrefix().end());

        // Add the compiler paths
        CompilerPtr compiler = BuildSettingsConfigST::Get()->GetCompiler(GetCompiler());
        if(compiler) {
            wxArrayString compilerPaths = compiler->GetDefaultIncludePaths();
            for(wxString& compilerPath : compilerPaths) {
                compilerPath.Prepend("-I");
            }
            options.insert(options.end(), compilerPaths.begin(), compilerPaths.end());
        }
        options.insert(options.end(), cclp.GetMacrosWithPrefix().begin(), cclp.GetMacrosWithPrefix().end());
        if(!cclp.GetStandard().empty()) { options.push_back(cclp.GetStandardWithPrefix()); }
        for(const wxString& opt : options) {
            compile_flags_txt << opt << "\n";
        }
    }

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
        if(!l.IsEmpty()) { s << l << "\n"; }
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

///===-------------------------------------
/// Workspace settings
///===-------------------------------------
clFileSystemWorkspaceSettings::clFileSystemWorkspaceSettings()
{
    Clear(); // Create with the default "Debug" target
}

clFileSystemWorkspaceSettings::~clFileSystemWorkspaceSettings() {}

JSONItem clFileSystemWorkspaceSettings::ToJSON(JSONItem& item) const
{
    item.addProperty("workspace_type", WORKSPACE_TYPE);
    item.addProperty("selected_config", m_selectedConfig);
    item.addProperty("name", m_name);
    JSONItem configs = JSONItem::createArray("configs");
    item.append(configs);
    for(const auto& config : m_configsMap) {
        configs.arrayAppend(config.second->ToJSON());
    }
    return item;
}

void clFileSystemWorkspaceSettings::FromJSON(const JSONItem& json)
{
    m_workspaceType = json.namedObject("workspace_type").toString(WORKSPACE_TYPE);
    m_selectedConfig = json.namedObject("selected_config").toString();
    m_name = json.namedObject("name").toString();
    JSONItem arrConfigs = json.namedObject("configs");
    int nCount = arrConfigs.arraySize();

    wxString firstConfig;
    bool selectedConfigFound = false;
    m_configsMap.clear();

    for(int i = 0; i < nCount; ++i) {
        clFileSystemWorkspaceConfig::Ptr_t conf(new clFileSystemWorkspaceConfig);
        conf->FromJSON(arrConfigs.arrayItem(i));
        if(firstConfig.empty()) { firstConfig = conf->GetName(); }
        if(!selectedConfigFound && (conf->GetName() == m_selectedConfig)) { selectedConfigFound = true; }
        m_configsMap.insert({ conf->GetName(), conf });
    }

    if(!selectedConfigFound && !firstConfig.IsEmpty()) { m_selectedConfig = firstConfig; }
}

bool clFileSystemWorkspaceSettings::Save(const wxFileName& filename)
{
    JSON root(cJSON_Object);
    JSONItem item = root.toElement();
    ToJSON(item);
    root.save(filename);
    return true;
}

bool clFileSystemWorkspaceSettings::Load(const wxFileName& filename)
{
    JSON root(filename);
    if(!root.isOk()) { return false; }
    FromJSON(root.toElement());
    return true;
}

bool clFileSystemWorkspaceSettings::AddConfig(const wxString& name)
{
    if(m_configsMap.count(name)) {
        // already exists
        return false;
    }
    clFileSystemWorkspaceConfig::Ptr_t conf(new clFileSystemWorkspaceConfig);
    conf->SetName(name);
    m_configsMap.insert({ name, conf });
    if(m_configsMap.size() == 1) { m_selectedConfig = conf->GetName(); }
    return true;
}

bool clFileSystemWorkspaceSettings::DeleteConfig(const wxString& name)
{
    if(m_configsMap.count(name) == 0) { return false; }
    m_configsMap.erase(name);
    if(m_selectedConfig == name) {
        m_selectedConfig.clear();
        if(!m_configsMap.empty()) { m_selectedConfig = m_configsMap.begin()->second->GetName(); }
    }
    return true;
}

clFileSystemWorkspaceConfig::Ptr_t clFileSystemWorkspaceSettings::GetSelectedConfig() const
{
    // sanity
    if(m_configsMap.empty()) { return clFileSystemWorkspaceConfig::Ptr_t(nullptr); }
    if(m_selectedConfig.empty() || (m_configsMap.count(m_selectedConfig) == 0)) {
        return clFileSystemWorkspaceConfig::Ptr_t(nullptr);
    }
    return m_configsMap.find(m_selectedConfig)->second;
}

clFileSystemWorkspaceConfig::Ptr_t clFileSystemWorkspaceSettings::GetConfig(const wxString& name) const
{
    if(name.empty()) { return GetSelectedConfig(); }
    if(m_configsMap.count(name) == 0) { return clFileSystemWorkspaceConfig::Ptr_t(nullptr); }
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
    if(m_configsMap.count(name) == 0) { return false; }
    m_selectedConfig = name;
    return true;
}

bool clFileSystemWorkspaceSettings::IsOk(const wxFileName& filename)
{
    JSON root(filename);
    if(!root.isOk()) { return false; }
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
