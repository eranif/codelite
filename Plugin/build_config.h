//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : build_config.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef BUILD_CONFIGURATION_H
#define BUILD_CONFIGURATION_H

#include "configuration_object.h"
#include "build_config_common.h"
#include <wx/arrstr.h>
#include <wx/string.h>
#include <list>
#include <map>
#include "compiler.h"
#include "codelite_exports.h"
#include "builder.h"

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif

class WXDLLIMPEXP_LE_SDK BuildCommand
{
    wxString m_command;
    bool m_enabled;

public:
    BuildCommand()
        : m_command(wxEmptyString)
        , m_enabled(false)
    {
    }

    BuildCommand(const wxString& command, bool enabled)
        : m_command(command)
        , m_enabled(enabled)
    {
    }

    ~BuildCommand() {}

    const wxString& GetCommand() const { return m_command; }
    bool GetEnabled() const { return m_enabled; }
    void SetCommand(const wxString& command) { m_command = command; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
};

typedef std::list<BuildCommand> BuildCommandList;

class WXDLLIMPEXP_SDK BuildConfig : public ConfObject
{
public:
    static const wxString OVERWRITE_GLOBAL_SETTINGS;
    static const wxString APPEND_TO_GLOBAL_SETTINGS;
    static const wxString PREPEND_GLOBAL_SETTINGS;
    typedef std::map<wxString, wxString> StringMap_t;

    enum ePCHPolicy {
        kPCHPolicyReplace = 0,
        kPCHPolicyAppend = 1,
    };

private:
    BuildConfigCommon m_commonConfig;
    wxString m_name;
    BuildCommandList m_preBuildCommands;
    BuildCommandList m_postBuildCommands;
    bool m_compilerRequired;
    bool m_linkerRequired;
    bool m_enableCustomBuild;
    wxString m_outputFile;
    wxString m_intermediateDirectory;
    wxString m_command;
    wxString m_commandArguments;
    wxString m_workingDirectory;
    wxString m_compilerType;
    wxString m_projectType;
    wxString m_customBuildCmd;
    wxString m_customCleanCmd;
    wxString m_customRebuildCmd;
    bool m_isResCmpNeeded;
    wxString m_debuggerType;
    wxString m_customPostBuildRule;
    wxString m_customPreBuildRule;
    wxString m_customBuildWorkingDir;
    bool m_pauseWhenExecEnds;
    wxString m_toolName;
    wxString m_makeGenerationCommand;
    wxString m_singleFileBuildCommand;
    wxString m_preprocessFileCommand;
    wxString m_debuggerStartupCmds;
    wxString m_debuggerPostRemoteConnectCmds;
    bool m_isDbgRemoteTarget;
    bool m_isDbgRemoteExtended;
    wxString m_dbgHostName;
    wxString m_dbgHostPort;
    StringMap_t m_customTargets;
    wxString m_debuggerPath;
    wxString m_buildCmpWithGlobalSettings;
    wxString m_buildLnkWithGlobalSettings;
    wxString m_buildResWithGlobalSettings;
    wxString m_precompiledHeader;
    wxString m_envVarSet;
    wxString m_dbgEnvSet;
    bool m_useSeparateDebugArgs;
    wxString m_debugArgs;
    wxString m_envvars;
    bool m_pchInCommandLine;
    wxString m_pchCompileFlags;
    wxString m_clangPPFlags;   // clang only pre-processors (useful when using a custom makefile)
    wxString m_clangCmpFlags;  // clang only compilation flags C++ (useful when using a custom makefile)
    wxString m_clangCmpFlagsC; // clang only compilation flags C   (useful when using a custom makefile)
    wxString m_ccSearchPaths;
    wxString m_ccPCH;
    bool m_clangC11;
    bool m_clangC14;
	bool m_clangC17;
    wxArrayString m_debuggerSearchPaths;
    bool m_isGUIProgram;
    bool m_isProjectEnabled;
    ePCHPolicy m_pchPolicy;
    wxString m_buildSystem;
    wxString m_buildSystemArguments;

public:
    BuildConfig(wxXmlNode* node);
    virtual ~BuildConfig();
    wxXmlNode* ToXml() const;
    BuildConfig* Clone() const;

    //--------------------------------
    // Setters / Getters
    //--------------------------------
    void SetPCHFlagsPolicy(BuildConfig::ePCHPolicy policy) { this->m_pchPolicy = policy; }
    BuildConfig::ePCHPolicy GetPCHFlagsPolicy() const { return this->m_pchPolicy; }
    void SetIsProjectEnabled(bool isProjectEnabled) { this->m_isProjectEnabled = isProjectEnabled; }
    bool IsProjectEnabled() const { return m_isProjectEnabled; }
    void SetIsGUIProgram(bool isGUIProgram) { this->m_isGUIProgram = isGUIProgram; }
    bool IsGUIProgram() const { return m_isGUIProgram; }
    void SetDebuggerSearchPaths(const wxArrayString& debuggerSearchPaths)
    {
        this->m_debuggerSearchPaths = debuggerSearchPaths;
    }
    const wxArrayString& GetDebuggerSearchPaths() const { return m_debuggerSearchPaths; }
    void SetClangC11(bool clangC11) { this->m_clangC11 = clangC11; }
    bool IsClangC11() const { return m_clangC11; }
    void SetClangC14(bool clangC14) { this->m_clangC14 = clangC14; }
    bool IsClangC14() const { return m_clangC14; }
	void SetClangC17(bool clangC17) { this->m_clangC17 = clangC17; }
    bool IsClangC17() const { return m_clangC17; }
    void SetClangCcPCH(const wxString& ccPCH) { this->m_ccPCH = ccPCH; }
    const wxString& GetClangCcPCH() const { return m_ccPCH; }
    void SetCcSearchPaths(const wxString& ccSearchPaths) { this->m_ccSearchPaths = ccSearchPaths; }
    void SetClangCmpFlags(const wxString& clangCmpFlags) { this->m_clangCmpFlags = clangCmpFlags; }
    void SetClangPPFlags(const wxString& clangPPFlags) { this->m_clangPPFlags = clangPPFlags; }
    const wxString& GetCcSearchPaths() const { return m_ccSearchPaths; }
    const wxString& GetClangCmpFlags() const { return m_clangCmpFlags; }

    void SetClangCmpFlagsC(const wxString& clangCmpFlagsC) { this->m_clangCmpFlagsC = clangCmpFlagsC; }
    const wxString& GetClangCmpFlagsC() const { return m_clangCmpFlagsC; }
    const wxString& GetClangPPFlags() const { return m_clangPPFlags; }
    void SetPchCompileFlags(const wxString& pchCompileFlags) { this->m_pchCompileFlags = pchCompileFlags; }
    const wxString& GetPchCompileFlags() const { return m_pchCompileFlags; }

    const wxString& GetAssmeblerOptions() const { return m_commonConfig.GetAssemblerOptions(); }
    void SetAssmeblerOptions(const wxString& options) { m_commonConfig.SetAssemblerOptions(options); }

    void SetPchInCommandLine(bool pchInCommandLine) { this->m_pchInCommandLine = pchInCommandLine; }
    bool GetPchInCommandLine() const { return m_pchInCommandLine; }
    void SetEnvvars(const wxString& envvars) { this->m_envvars = envvars; }
    const wxString& GetEnvvars() const { return m_envvars; }
    wxString GetPreprocessor() const;
    void GetPreprocessor(wxArrayString& arr) { m_commonConfig.GetPreprocessor(arr); }
    void SetPreprocessor(const wxString& prepr);

    const wxString& GetCompilerType() const { return m_compilerType; }

    CompilerPtr GetCompiler() const;

    void SetDebugArgs(const wxString& debugArgs) { this->m_debugArgs = debugArgs; }
    void SetUseSeparateDebugArgs(bool useSeparateDebugArgs) { this->m_useSeparateDebugArgs = useSeparateDebugArgs; }
    const wxString& GetDebugArgs() const { return m_debugArgs; }
    bool GetUseSeparateDebugArgs() const { return m_useSeparateDebugArgs; }
    void SetCompilerType(const wxString& cmpType) { m_compilerType = cmpType; }
    const wxString& GetDebuggerType() const { return m_debuggerType; }
    void SetDebuggerType(const wxString& type) { m_debuggerType = type; }

    wxString GetIncludePath() const;
    const wxString& GetCompileOptions() const { return m_commonConfig.GetCompileOptions(); }
    const wxString& GetCCompileOptions() const { return m_commonConfig.GetCCompileOptions(); }
    const wxString& GetLinkOptions() const { return m_commonConfig.GetLinkOptions(); }
    wxString GetLibraries() const;
    wxString GetLibPath() const;
    void GetPreBuildCommands(BuildCommandList& cmds) { cmds = m_preBuildCommands; }
    void GetPostBuildCommands(BuildCommandList& cmds) { cmds = m_postBuildCommands; }
    const wxString& GetName() const { return m_name; }
    bool IsCompilerRequired() const { return m_compilerRequired; }
    bool IsLinkerRequired() const { return m_linkerRequired; }
    wxString GetOutputFileName() const;
    wxString GetIntermediateDirectory() const;
    const wxString& GetCommand() const { return m_command; }
    const wxString& GetCommandArguments() const { return m_commandArguments; }
    wxString GetWorkingDirectory() const;
    bool IsCustomBuild() const { return m_enableCustomBuild; }
    const wxString& GetCustomBuildCmd() const { return m_customBuildCmd; }
    const wxString& GetCustomCleanCmd() const { return m_customCleanCmd; }
    const wxString& GetCustomRebuildCmd() const { return m_customRebuildCmd; }
    void SetIncludePath(const wxArrayString& paths) { m_commonConfig.SetIncludePath(paths); }
    void SetIncludePath(const wxString& path);
    void SetLibraries(const wxString& libs);

    void SetLibPath(const wxString& path);
    void SetCompileOptions(const wxString& options) { m_commonConfig.SetCompileOptions(options); }
    void SetCCompileOptions(const wxString& options) { m_commonConfig.SetCCompileOptions(options); }
    void SetLinkOptions(const wxString& options) { m_commonConfig.SetLinkOptions(options); }
    void SetPreBuildCommands(const BuildCommandList& cmds) { m_preBuildCommands = cmds; }
    void SetPostBuildCommands(const BuildCommandList& cmds) { m_postBuildCommands = cmds; }
    void SetLibraries(const wxArrayString& libs) { m_commonConfig.SetLibraries(libs); }
    void SetLibPath(const wxArrayString& libPaths) { m_commonConfig.SetLibPath(libPaths); }
    void SetName(const wxString& name) { m_name = name; }
    void SetCompilerRequired(bool required) { m_compilerRequired = required; }
    void SetLinkerRequired(bool required) { m_linkerRequired = required; }
    void SetOutputFileName(const wxString& name) { m_outputFile = name; }
    void SetIntermediateDirectory(const wxString& dir) { m_intermediateDirectory = dir; }
    void SetCommand(const wxString& cmd) { m_command = cmd; }
    void SetCommandArguments(const wxString& cmdArgs) { m_commandArguments = cmdArgs; }
    void SetWorkingDirectory(const wxString& dir) { m_workingDirectory = dir; }
    void SetCustomBuildCmd(const wxString& cmd) { m_customBuildCmd = cmd; }
    void SetCustomCleanCmd(const wxString& cmd) { m_customCleanCmd = cmd; }
    void SetCustomRebuildCmd(const wxString& cmd) { m_customRebuildCmd = cmd; }
    void EnableCustomBuild(bool enable) { m_enableCustomBuild = enable; }

    void SetResCompilerRequired(bool required) { m_isResCmpNeeded = required; }
    bool IsResCompilerRequired() const { return m_isResCmpNeeded; }

    void SetResCmpIncludePath(const wxString& path) { m_commonConfig.SetResCmpIncludePath(path); }
    const wxString& GetResCmpIncludePath() const { return m_commonConfig.GetResCmpIncludePath(); }

    void SetResCmpOptions(const wxString& options) { m_commonConfig.SetResCmpOptions(options); }
    const wxString& GetResCompileOptions() const { return m_commonConfig.GetResCompileOptions(); }

    // special custom rules
    wxString GetPreBuildCustom() const { return m_customPreBuildRule; }
    wxString GetPostBuildCustom() const { return m_customPostBuildRule; }

    void SetPreBuildCustom(const wxString& rule) { m_customPreBuildRule = rule; }
    void SetPostBuildCustom(const wxString& rule) { m_customPostBuildRule = rule; }

    void SetCustomBuildWorkingDir(const wxString& customBuildWorkingDir)
    {
        this->m_customBuildWorkingDir = customBuildWorkingDir;
    }
    const wxString& GetCustomBuildWorkingDir() const { return m_customBuildWorkingDir; }

    void SetPauseWhenExecEnds(const bool& pauseWhenExecEnds) { this->m_pauseWhenExecEnds = pauseWhenExecEnds; }
    const bool& GetPauseWhenExecEnds() const { return m_pauseWhenExecEnds; }

    void SetMakeGenerationCommand(const wxString& makeGenerationCommand)
    {
        this->m_makeGenerationCommand = makeGenerationCommand;
    }
    void SetToolName(const wxString& toolName) { this->m_toolName = toolName; }

    const wxString& GetMakeGenerationCommand() const { return m_makeGenerationCommand; }
    const wxString& GetToolName() const { return m_toolName; }

    void SetSingleFileBuildCommand(const wxString& singleFileBuildCommand)
    {
        this->m_singleFileBuildCommand = singleFileBuildCommand;
    }
    const wxString& GetSingleFileBuildCommand() const { return m_singleFileBuildCommand; }

    void SetPreprocessFileCommand(const wxString& preprocessFileCommand)
    {
        this->m_preprocessFileCommand = preprocessFileCommand;
    }
    const wxString& GetPreprocessFileCommand() const { return m_preprocessFileCommand; }

    const wxString& GetProjectType() const { return m_projectType; }
    void SetProjectType(const wxString& projectType) { m_projectType = projectType; }

    void SetDebuggerStartupCmds(const wxString& debuggerStartupCmds)
    {
        this->m_debuggerStartupCmds = debuggerStartupCmds;
    }
    const wxString& GetDebuggerStartupCmds() const { return m_debuggerStartupCmds; }

    void SetIsDbgRemoteTarget(const bool& isDbgRemoteTarget) { this->m_isDbgRemoteTarget = isDbgRemoteTarget; }
    const bool& GetIsDbgRemoteTarget() const { return m_isDbgRemoteTarget; }

    void SetDbgHostName(const wxString& dbgHostName) { this->m_dbgHostName = dbgHostName; }
    void SetDbgHostPort(const wxString& dbgHostPort) { this->m_dbgHostPort = dbgHostPort; }

    const wxString& GetDbgHostName() const { return m_dbgHostName; }
    const wxString& GetDbgHostPort() const { return m_dbgHostPort; }

    void SetIsDbgRemoteExtended(const bool& isDbgRemoteExtended) { this->m_isDbgRemoteExtended = isDbgRemoteExtended; }
    const bool& GetIsDbgRemoteExtended() const { return m_isDbgRemoteExtended; }

    void SetCustomTargets(const BuildConfig::StringMap_t& customTargets) { this->m_customTargets = customTargets; }
    const BuildConfig::StringMap_t& GetCustomTargets() const { return m_customTargets; }
    void SetDebuggerPath(const wxString& debuggerPath) { this->m_debuggerPath = debuggerPath; }
    const wxString& GetDebuggerPath() const { return m_debuggerPath; }

    void SetDebuggerPostRemoteConnectCmds(const wxString& debuggerPostRemoteConnectCmds)
    {
        this->m_debuggerPostRemoteConnectCmds = debuggerPostRemoteConnectCmds;
    }
    const wxString& GetDebuggerPostRemoteConnectCmds() const { return m_debuggerPostRemoteConnectCmds; }

    const wxString& GetBuildCmpWithGlobalSettings() const { return m_buildCmpWithGlobalSettings; }
    void SetBuildCmpWithGlobalSettings(const wxString& buildType) { m_buildCmpWithGlobalSettings = buildType; }

    const wxString& GetBuildLnkWithGlobalSettings() const { return m_buildLnkWithGlobalSettings; }
    void SetBuildLnkWithGlobalSettings(const wxString& buildType) { m_buildLnkWithGlobalSettings = buildType; }

    const wxString& GetBuildResWithGlobalSettings() const { return m_buildResWithGlobalSettings; }
    void SetBuildResWithGlobalSettings(const wxString& buildType) { m_buildResWithGlobalSettings = buildType; }

    const BuildConfigCommon& GetCommonConfiguration() const { return m_commonConfig; }
    void SetPrecompiledHeader(const wxString& precompiledHeader) { this->m_precompiledHeader = precompiledHeader; }
    const wxString& GetPrecompiledHeader() const { return m_precompiledHeader; }

    void SetDbgEnvSet(const wxString& dbgEnvSet) { this->m_dbgEnvSet = dbgEnvSet; }
    void SetEnvVarSet(const wxString& envVarSet) { this->m_envVarSet = envVarSet; }
    const wxString& GetDbgEnvSet() const { return m_dbgEnvSet; }
    const wxString& GetEnvVarSet() const { return m_envVarSet; }

    void SetBuildSystem(const wxString& buildSystem) { this->m_buildSystem = buildSystem; }
    const wxString& GetBuildSystem() const { return m_buildSystem; }
    void SetBuildSystemArguments(const wxString& buildSystemArguments)
    {
        this->m_buildSystemArguments = buildSystemArguments;
    }
    const wxString& GetBuildSystemArguments() const { return m_buildSystemArguments; }
    
    /**
     * @brief return the build defined for this build configuration
     * @return 
     */
    BuilderPtr GetBuilder();
};

typedef SmartPtr<BuildConfig> BuildConfigPtr;

#endif // BUILD_CONFIGURATION_H
