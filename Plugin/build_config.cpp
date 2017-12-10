//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : build_config.cpp
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
#include "build_config.h"
#include "xmlutils.h"
#include "wx/tokenzr.h"
#include "macros.h"
#include "project.h"
#include "editor_config.h"
#include "build_settings_config.h"
#include "debuggermanager.h"
#include "globals.h"
#include "wx_xml_compatibility.h"
#include <wx/tokenzr.h>
#include "buildmanager.h"

const wxString BuildConfig::OVERWRITE_GLOBAL_SETTINGS = "overwrite";
const wxString BuildConfig::APPEND_TO_GLOBAL_SETTINGS = "append";
const wxString BuildConfig::PREPEND_GLOBAL_SETTINGS = "prepend";

BuildConfig::BuildConfig(wxXmlNode* node)
    : m_commonConfig(node)
    , m_useSeparateDebugArgs(false)
    , m_pchInCommandLine(false)
    , m_clangC11(false)
    , m_clangC14(false)
    , m_isGUIProgram(false)
    , m_isProjectEnabled(true)
    , m_pchPolicy(BuildConfig::kPCHPolicyReplace)
    , m_buildSystem("Default")
{
    if(node) {
        m_name = XmlUtils::ReadString(node, wxT("Name"));
        m_compilerType = XmlUtils::ReadString(node, wxT("CompilerType"));
        m_debuggerType = XmlUtils::ReadString(node, wxT("DebuggerType"));
        m_projectType = XmlUtils::ReadString(node, wxT("Type"));
        m_buildCmpWithGlobalSettings =
            XmlUtils::ReadString(node, wxT("BuildCmpWithGlobalSettings"), APPEND_TO_GLOBAL_SETTINGS);
        m_buildLnkWithGlobalSettings =
            XmlUtils::ReadString(node, wxT("BuildLnkWithGlobalSettings"), APPEND_TO_GLOBAL_SETTINGS);
        m_buildResWithGlobalSettings =
            XmlUtils::ReadString(node, wxT("BuildResWithGlobalSettings"), APPEND_TO_GLOBAL_SETTINGS);

        wxXmlNode* buildSystem = XmlUtils::FindFirstByTagName(node, wxT("BuildSystem"));
        if(buildSystem) {
            m_buildSystem = XmlUtils::ReadString(buildSystem, "Name", m_buildSystem);
            m_buildSystemArguments = buildSystem->GetNodeContent();
        }

        wxXmlNode* completion = XmlUtils::FindFirstByTagName(node, wxT("Completion"));
        if(completion) {
            wxXmlNode* search_paths = XmlUtils::FindFirstByTagName(completion, wxT("SearchPaths"));
            if(search_paths) {
                m_ccSearchPaths = search_paths->GetNodeContent();
                m_ccSearchPaths.Trim().Trim(false);
            }

            wxXmlNode* clang_pp = XmlUtils::FindFirstByTagName(completion, wxT("ClangPP"));
            if(clang_pp) {
                m_clangPPFlags = clang_pp->GetNodeContent();
                m_clangPPFlags.Trim().Trim(false);
            }

            wxXmlNode* clang_cmp_flags = XmlUtils::FindFirstByTagName(completion, wxT("ClangCmpFlags"));
            if(clang_cmp_flags) {
                m_clangCmpFlags = clang_cmp_flags->GetNodeContent();
                m_clangCmpFlags.Trim().Trim(false);
            }

            wxXmlNode* clang_c_cmp_flags = XmlUtils::FindFirstByTagName(completion, wxT("ClangCmpFlagsC"));
            if(clang_c_cmp_flags) {
                m_clangCmpFlagsC = clang_c_cmp_flags->GetNodeContent();
                m_clangCmpFlagsC.Trim().Trim(false);
            }

            wxXmlNode* clang_pch = XmlUtils::FindFirstByTagName(completion, wxT("ClangPCH"));
            if(clang_pch) {
                m_ccPCH = clang_pch->GetNodeContent();
                m_ccPCH.Trim().Trim(false);
            }

            m_clangC11 = XmlUtils::ReadBool(completion, wxT("EnableCpp11"));
            m_clangC14 = XmlUtils::ReadBool(completion, wxT("EnableCpp14"));
        }

        wxXmlNode* compile = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
        if(compile) {
            m_compilerRequired = XmlUtils::ReadBool(compile, wxT("Required"), true);
            m_precompiledHeader = XmlUtils::ReadString(compile, wxT("PreCompiledHeader"));
            m_pchInCommandLine = XmlUtils::ReadBool(compile, wxT("PCHInCommandLine"), false);
            m_pchCompileFlags = XmlUtils::ReadString(compile, wxT("PCHFlags"));
            m_pchPolicy = (ePCHPolicy)XmlUtils::ReadLong(compile, "PCHFlagsPolicy", m_pchPolicy);
        }

        wxXmlNode* linker = XmlUtils::FindFirstByTagName(node, wxT("Linker"));
        if(linker) {
            m_linkerRequired = XmlUtils::ReadBool(linker, wxT("Required"), true);
        }

        wxXmlNode* resCmp = XmlUtils::FindFirstByTagName(node, wxT("ResourceCompiler"));
        if(resCmp) {
            m_isResCmpNeeded = XmlUtils::ReadBool(resCmp, wxT("Required"), true);
        }

        // read the postbuild commands
        wxXmlNode* debugger = XmlUtils::FindFirstByTagName(node, wxT("Debugger"));
        m_isDbgRemoteTarget = false;

        if(debugger) {
            m_isDbgRemoteTarget = XmlUtils::ReadBool(debugger, wxT("IsRemote"));
            m_dbgHostName = XmlUtils::ReadString(debugger, wxT("RemoteHostName"));
            m_dbgHostPort = XmlUtils::ReadString(debugger, wxT("RemoteHostPort"));
            m_debuggerPath = XmlUtils::ReadString(debugger, wxT("DebuggerPath"));
            m_isDbgRemoteExtended = XmlUtils::ReadBool(debugger, wxT("IsExtended"));

            wxXmlNode* child = debugger->GetChildren();
            while(child) {
                if(child->GetName() == wxT("StartupCommands")) {
                    m_debuggerStartupCmds = child->GetNodeContent();

                } else if(child->GetName() == wxT("PostConnectCommands")) {
                    m_debuggerPostRemoteConnectCmds = child->GetNodeContent();

                } else if(child->GetName() == "DebuggerSearchPaths") {
                    wxArrayString searchPaths = ::wxStringTokenize(child->GetNodeContent(), "\r\n", wxTOKEN_STRTOK);
                    m_debuggerSearchPaths.swap(searchPaths);
                }

                child = child->GetNext();
            }
        }

        // read the prebuild commands
        wxXmlNode* preBuild = XmlUtils::FindFirstByTagName(node, wxT("PreBuild"));
        if(preBuild) {
            wxXmlNode* child = preBuild->GetChildren();
            while(child) {
                if(child->GetName() == wxT("Command")) {
                    bool enabled = XmlUtils::ReadBool(child, wxT("Enabled"));

                    BuildCommand cmd(child->GetNodeContent(), enabled);
                    m_preBuildCommands.push_back(cmd);
                }
                child = child->GetNext();
            }
        }
        // read the postbuild commands
        wxXmlNode* postBuild = XmlUtils::FindFirstByTagName(node, wxT("PostBuild"));
        if(postBuild) {
            wxXmlNode* child = postBuild->GetChildren();
            while(child) {
                if(child->GetName() == wxT("Command")) {
                    bool enabled = XmlUtils::ReadBool(child, wxT("Enabled"));
                    BuildCommand cmd(child->GetNodeContent(), enabled);
                    m_postBuildCommands.push_back(cmd);
                }
                child = child->GetNext();
            }
        }

        SetEnvVarSet(USE_WORKSPACE_ENV_VAR_SET);
        SetDbgEnvSet(USE_GLOBAL_SETTINGS);

        // read the environment page
        wxXmlNode* envNode = XmlUtils::FindFirstByTagName(node, wxT("Environment"));
        if(envNode) {
            SetEnvVarSet(XmlUtils::ReadString(envNode, wxT("EnvVarSetName")));
            SetDbgEnvSet(XmlUtils::ReadString(envNode, wxT("DbgSetName")));
            m_envvars = envNode->GetNodeContent();
        }

        wxXmlNode* customBuild = XmlUtils::FindFirstByTagName(node, wxT("CustomBuild"));
        if(customBuild) {
            m_enableCustomBuild = XmlUtils::ReadBool(customBuild, wxT("Enabled"), false);
            wxXmlNode* child = customBuild->GetChildren();
            while(child) {
                if(child->GetName() == wxT("BuildCommand")) {
                    m_customBuildCmd = child->GetNodeContent();
                } else if(child->GetName() == wxT("CleanCommand")) {
                    m_customCleanCmd = child->GetNodeContent();
                } else if(child->GetName() == wxT("RebuildCommand")) {
                    m_customRebuildCmd = child->GetNodeContent();
                } else if(child->GetName() == wxT("SingleFileCommand")) {
                    m_singleFileBuildCommand = child->GetNodeContent();
                } else if(child->GetName() == wxT("PreprocessFileCommand")) {
                    m_preprocessFileCommand = child->GetNodeContent();
                } else if(child->GetName() == wxT("WorkingDirectory")) {
                    m_customBuildWorkingDir = child->GetNodeContent();
                } else if(child->GetName() == wxT("ThirdPartyToolName")) {
                    m_toolName = child->GetNodeContent();
                } else if(child->GetName() == wxT("MakefileGenerationCommand")) {
                    m_makeGenerationCommand = child->GetNodeContent();
                } else if(child->GetName() == wxT("Target")) {
                    wxString target_name = child->GetPropVal(wxT("Name"), wxT(""));
                    wxString target_cmd = child->GetNodeContent();
                    if(target_name.IsEmpty() == false) {
                        m_customTargets[target_name] = target_cmd;
                    }
                }
                child = child->GetNext();
            }
        } else {
            m_enableCustomBuild = false;
        }

        wxXmlNode* customPreBuild = XmlUtils::FindFirstByTagName(node, wxT("AdditionalRules"));
        if(customPreBuild) {
            wxXmlNode* child = customPreBuild->GetChildren();
            while(child) {
                if(child->GetName() == wxT("CustomPreBuild")) {
                    m_customPreBuildRule = child->GetNodeContent();
                    m_customPreBuildRule.Trim().Trim(false);

                } else if(child->GetName() == wxT("CustomPostBuild")) {
                    m_customPostBuildRule = child->GetNodeContent();
                    m_customPostBuildRule.Trim().Trim(false);
                }
                child = child->GetNext();
            }
        }

        wxXmlNode* general = XmlUtils::FindFirstByTagName(node, wxT("General"));
        if(general) {
            m_outputFile = XmlUtils::ReadString(general, wxT("OutputFile"));
            m_intermediateDirectory = XmlUtils::ReadString(general, wxT("IntermediateDirectory"), wxT("."));
            m_command = XmlUtils::ReadString(general, wxT("Command"));
            m_commandArguments = XmlUtils::ReadString(general, wxT("CommandArguments"));
            m_workingDirectory = XmlUtils::ReadString(general, wxT("WorkingDirectory"), wxT("."));
            m_pauseWhenExecEnds = XmlUtils::ReadBool(general, wxT("PauseExecWhenProcTerminates"), true);
            m_useSeparateDebugArgs = XmlUtils::ReadBool(general, wxT("UseSeparateDebugArgs"), false);
            m_debugArgs = XmlUtils::ReadString(general, wxT("DebugArguments"));
            m_isGUIProgram = XmlUtils::ReadBool(general, "IsGUIProgram", false);
            m_isProjectEnabled = XmlUtils::ReadBool(general, "IsEnabled", true);
        }

    } else {

        // create default project settings
        m_commonConfig.SetCompileOptions(wxT("-g -Wall"));
        m_commonConfig.SetLinkOptions(wxT("-O0"));
        m_commonConfig.SetLibPath(wxT(".;Debug"));

        m_name = wxT("Debug");
        m_compilerRequired = true;
        m_linkerRequired = true;
        m_intermediateDirectory = wxT("./Debug");
        m_workingDirectory = wxT("./Debug");
        m_projectType = PROJECT_TYPE_EXECUTABLE;
        m_enableCustomBuild = false;
        m_customBuildCmd = wxEmptyString;
        m_customCleanCmd = wxEmptyString;
        m_isResCmpNeeded = false;
        m_customPostBuildRule = wxEmptyString;
        m_customPreBuildRule = wxEmptyString;
        m_makeGenerationCommand = wxEmptyString;
        m_toolName = wxEmptyString;
        m_singleFileBuildCommand = wxEmptyString;
        m_preprocessFileCommand = wxEmptyString;
        m_debuggerStartupCmds = wxEmptyString;
        m_debuggerPostRemoteConnectCmds = wxEmptyString;
        m_isDbgRemoteExtended = false;
        m_isDbgRemoteTarget = false;
        m_useSeparateDebugArgs = false;
        m_debugArgs = wxEmptyString;

        SetEnvVarSet(wxT("<Use Workspace Settings>"));
        SetDbgEnvSet(wxT("<Use Global Settings>"));

        BuildSettingsConfigCookie cookie;
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
        if(cmp) {
            m_compilerType = cmp->GetName();
        }
        wxArrayString dbgs = DebuggerMgr::Get().GetAvailableDebuggers();
        if(dbgs.GetCount() > 0) {
            m_debuggerType = dbgs.Item(0);
        }
        m_buildCmpWithGlobalSettings = APPEND_TO_GLOBAL_SETTINGS;
        m_buildLnkWithGlobalSettings = APPEND_TO_GLOBAL_SETTINGS;
        m_buildResWithGlobalSettings = APPEND_TO_GLOBAL_SETTINGS;
    }
}

BuildConfig::~BuildConfig() {}

BuildConfig* BuildConfig::Clone() const
{
    wxXmlNode* node = ToXml();
    BuildConfig* cloned = new BuildConfig(node);
    delete node;
    return cloned;
}

wxXmlNode* BuildConfig::ToXml() const
{
    // Create the common nodes
    wxXmlNode* node = m_commonConfig.ToXml();

    node->AddProperty(wxT("Name"), m_name);
    node->AddProperty(wxT("CompilerType"), m_compilerType);
    node->AddProperty(wxT("DebuggerType"), m_debuggerType);
    node->AddProperty(wxT("Type"), m_projectType);
    node->AddProperty(wxT("BuildCmpWithGlobalSettings"), m_buildCmpWithGlobalSettings);
    node->AddProperty(wxT("BuildLnkWithGlobalSettings"), m_buildLnkWithGlobalSettings);
    node->AddProperty(wxT("BuildResWithGlobalSettings"), m_buildResWithGlobalSettings);

    wxXmlNode* compile = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
    if(compile) {
        compile->AddProperty(wxT("Required"), BoolToString(m_compilerRequired));
        compile->AddProperty(wxT("PreCompiledHeader"), m_precompiledHeader);
        compile->AddProperty(wxT("PCHInCommandLine"), BoolToString(m_pchInCommandLine));
        compile->AddProperty(wxT("PCHFlags"), m_pchCompileFlags);
        compile->AddProperty("PCHFlagsPolicy", wxString() << (int)m_pchPolicy);
    }

    wxXmlNode* link = XmlUtils::FindFirstByTagName(node, wxT("Linker"));
    if(link) {
        link->AddProperty(wxT("Required"), BoolToString(m_linkerRequired));
    }

    wxXmlNode* resCmp = XmlUtils::FindFirstByTagName(node, wxT("ResourceCompiler"));
    if(resCmp) {
        resCmp->AddProperty(wxT("Required"), BoolToString(m_isResCmpNeeded));
    }

    wxXmlNode* general = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("General"));
    general->AddProperty(wxT("OutputFile"), m_outputFile);
    general->AddProperty(wxT("IntermediateDirectory"), m_intermediateDirectory);
    general->AddProperty(wxT("Command"), m_command);
    general->AddProperty(wxT("CommandArguments"), m_commandArguments);
    general->AddProperty(wxT("UseSeparateDebugArgs"), BoolToString(m_useSeparateDebugArgs));
    general->AddProperty(wxT("DebugArguments"), m_debugArgs);
    general->AddProperty(wxT("WorkingDirectory"), m_workingDirectory);
    general->AddProperty(wxT("PauseExecWhenProcTerminates"), BoolToString(m_pauseWhenExecEnds));
    general->AddProperty("IsGUIProgram", BoolToString(m_isGUIProgram));
    general->AddProperty("IsEnabled", BoolToString(m_isProjectEnabled));
    node->AddChild(general);

    wxXmlNode* buildSystem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("BuildSystem"));
    buildSystem->AddProperty("Name", m_buildSystem);
    XmlUtils::SetNodeContent(buildSystem, m_buildSystemArguments);
    node->AddChild(buildSystem);

    wxXmlNode* debugger = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Debugger"));
    debugger->AddProperty(wxT("IsRemote"), BoolToString(m_isDbgRemoteTarget));
    debugger->AddProperty(wxT("RemoteHostName"), m_dbgHostName);
    debugger->AddProperty(wxT("RemoteHostPort"), m_dbgHostPort);
    debugger->AddProperty(wxT("DebuggerPath"), m_debuggerPath);
    debugger->AddProperty(wxT("IsExtended"), BoolToString(m_isDbgRemoteExtended));

    wxXmlNode* envNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Environment"));
    envNode->AddProperty(wxT("EnvVarSetName"), GetEnvVarSet());
    envNode->AddProperty(wxT("DbgSetName"), GetDbgEnvSet());

    // Add CDATA section with project environment variables
    wxXmlNode* envContent = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxEmptyString, m_envvars);
    envNode->AddChild(envContent);
    node->AddChild(envNode);

    wxXmlNode* dbgStartupCommands = new wxXmlNode(debugger, wxXML_ELEMENT_NODE, wxT("StartupCommands"));
    XmlUtils::SetNodeContent(dbgStartupCommands, m_debuggerStartupCmds);

    wxXmlNode* dbgPostConnectCommands = new wxXmlNode(debugger, wxXML_ELEMENT_NODE, wxT("PostConnectCommands"));
    XmlUtils::SetNodeContent(dbgPostConnectCommands, m_debuggerPostRemoteConnectCmds);

    wxXmlNode* dbgSearchPaths = new wxXmlNode(debugger, wxXML_ELEMENT_NODE, wxT("DebuggerSearchPaths"));
    XmlUtils::SetNodeContent(dbgSearchPaths, ::wxImplode(m_debuggerSearchPaths, "\n"));

    node->AddChild(debugger);

    // add prebuild commands
    wxXmlNode* preBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PreBuild"));
    node->AddChild(preBuild);

    BuildCommandList::const_iterator iter = m_preBuildCommands.begin();
    for(; iter != m_preBuildCommands.end(); iter++) {
        wxXmlNode* command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
        command->AddProperty(wxT("Enabled"), BoolToString(iter->GetEnabled()));
        XmlUtils::SetNodeContent(command, iter->GetCommand());
        preBuild->AddChild(command);
    }

    // add postbuild commands
    wxXmlNode* postBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PostBuild"));
    node->AddChild(postBuild);
    iter = m_postBuildCommands.begin();
    for(; iter != m_postBuildCommands.end(); iter++) {
        wxXmlNode* command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
        command->AddProperty(wxT("Enabled"), BoolToString(iter->GetEnabled()));
        XmlUtils::SetNodeContent(command, iter->GetCommand());
        postBuild->AddChild(command);
    }

    // add postbuild commands
    wxXmlNode* customBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CustomBuild"));
    node->AddChild(customBuild);
    customBuild->AddProperty(wxT("Enabled"), BoolToString(m_enableCustomBuild));

    // add the working directory of the custom build
    wxXmlNode* customBuildWd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("WorkingDirectory"));
    XmlUtils::SetNodeContent(customBuildWd, m_customBuildWorkingDir);

    // add the makefile generation command
    wxXmlNode* toolName = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("ThirdPartyToolName"));
    XmlUtils::SetNodeContent(toolName, m_toolName);

    // add the makefile generation command
    wxXmlNode* makeGenCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("MakefileGenerationCommand"));
    XmlUtils::SetNodeContent(makeGenCmd, m_makeGenerationCommand);

    // add the makefile generation command
    wxXmlNode* singleFileCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("SingleFileCommand"));
    XmlUtils::SetNodeContent(singleFileCmd, m_singleFileBuildCommand);

    // add the makefile generation command
    wxXmlNode* preprocessFileCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("PreprocessFileCommand"));
    XmlUtils::SetNodeContent(preprocessFileCmd, m_preprocessFileCommand);

    // add build and clean commands
    wxXmlNode* bldCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("BuildCommand"));
    XmlUtils::SetNodeContent(bldCmd, m_customBuildCmd);

    wxXmlNode* clnCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("CleanCommand"));
    XmlUtils::SetNodeContent(clnCmd, m_customCleanCmd);

    wxXmlNode* rebldCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("RebuildCommand"));
    XmlUtils::SetNodeContent(rebldCmd, m_customRebuildCmd);

    // add all 'Targets'
    std::map<wxString, wxString>::const_iterator ir = m_customTargets.begin();
    for(; ir != m_customTargets.end(); ir++) {
        wxString target_name = ir->first;
        wxString target_cmd = ir->second;

        wxXmlNode* customTarget = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("Target"));
        customTarget->AddProperty(wxT("Name"), target_name);
        XmlUtils::SetNodeContent(customTarget, target_cmd);
    }

    // add the additional rules
    wxXmlNode* additionalCmds = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("AdditionalRules"));
    node->AddChild(additionalCmds);

    wxXmlNode* preCmd = new wxXmlNode(additionalCmds, wxXML_ELEMENT_NODE, wxT("CustomPreBuild"));
    XmlUtils::SetNodeContent(preCmd, m_customPreBuildRule);
    wxXmlNode* postCmd = new wxXmlNode(additionalCmds, wxXML_ELEMENT_NODE, wxT("CustomPostBuild"));
    XmlUtils::SetNodeContent(postCmd, m_customPostBuildRule);

    // Set the completion flags
    wxXmlNode* completion = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Completion"));
    node->AddChild(completion);
    completion->AddProperty(wxT("EnableCpp11"), BoolToString(m_clangC11));
    completion->AddProperty(wxT("EnableCpp14"), BoolToString(m_clangC14));

    wxXmlNode* search_paths = new wxXmlNode(completion, wxXML_ELEMENT_NODE, wxT("SearchPaths"));
    XmlUtils::SetNodeContent(search_paths, m_ccSearchPaths);

    wxXmlNode* clang_pp = new wxXmlNode(completion, wxXML_ELEMENT_NODE, wxT("ClangPP"));
    XmlUtils::SetNodeContent(clang_pp, m_clangPPFlags);

    wxXmlNode* clang_cmp_flags = new wxXmlNode(completion, wxXML_ELEMENT_NODE, wxT("ClangCmpFlags"));
    XmlUtils::SetNodeContent(clang_cmp_flags, m_clangCmpFlags);

    wxXmlNode* clang_c_cmp_flags = new wxXmlNode(completion, wxXML_ELEMENT_NODE, wxT("ClangCmpFlagsC"));
    XmlUtils::SetNodeContent(clang_c_cmp_flags, m_clangCmpFlagsC);

    return node;
}

void BuildConfig::SetPreprocessor(const wxString& pre) { m_commonConfig.SetPreprocessor(pre); }

void BuildConfig::SetIncludePath(const wxString& path) { m_commonConfig.SetIncludePath(path); }

void BuildConfig::SetLibraries(const wxString& libs) { m_commonConfig.SetLibraries(libs); }

void BuildConfig::SetLibPath(const wxString& paths) { m_commonConfig.SetLibPath(paths); }

wxString BuildConfig::GetLibPath() const { return m_commonConfig.GetLibPath(); }

wxString BuildConfig::GetLibraries() const { return m_commonConfig.GetLibraries(); }

wxString BuildConfig::GetIncludePath() const { return m_commonConfig.GetIncludePath(); }

wxString BuildConfig::GetPreprocessor() const { return m_commonConfig.GetPreprocessor(); }

wxString BuildConfig::GetOutputFileName() const { return NormalizePath(m_outputFile); }

wxString BuildConfig::GetIntermediateDirectory() const { return NormalizePath(m_intermediateDirectory); }

wxString BuildConfig::GetWorkingDirectory() const { return NormalizePath(m_workingDirectory); }

CompilerPtr BuildConfig::GetCompiler() const { return BuildSettingsConfigST::Get()->GetCompiler(GetCompilerType()); }

BuilderPtr BuildConfig::GetBuilder() { return BuildManagerST::Get()->GetBuilder(GetBuildSystem()); }
