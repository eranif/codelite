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
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
#include <wx/arrstr.h>
#include "wx/string.h"
#include <list>

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif 

class WXDLLIMPEXP_LE_SDK BuildCommand {
	wxString m_command;
	bool m_enabled;

public:
	BuildCommand()
		: m_command(wxEmptyString)
		, m_enabled(false)
	{
	}

	BuildCommand(const wxString &command, bool enabled)
		: m_command(command)
		, m_enabled(enabled)
	{}

	~BuildCommand()
	{}

	const wxString &GetCommand() const {return m_command;}
	bool GetEnabled() const { return m_enabled;}
	void SetCommand(const wxString &command) {m_command = command;}
	void SetEnabled(bool enabled) {m_enabled = enabled;}
};

typedef std::list<BuildCommand> BuildCommandList;

class WXDLLIMPEXP_LE_SDK BuildConfig : public ConfObject {
	wxString m_name;
	wxArrayString m_includePath;
	wxString m_compileOptions;
	wxString m_linkOptions;
	wxArrayString m_libs;
	wxArrayString m_libPath;
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
	wxArrayString m_preprocessor;
	wxString m_customBuildCmd;
	wxString m_customCleanCmd;
	wxString m_resCompileOptions;
	wxString m_resCmpIncludePath;
	bool m_isResCmpNeeded;
	wxString m_debuggerType;
	wxString m_customPostBuildRule;
	wxString m_customPreBuildRule;
	wxString m_customBuildWorkingDir;
	bool m_pauseWhenExecEnds;
	wxString m_toolName;
	wxString m_makeGenerationCommand;
	wxString m_singleFileBuildCommand;
	
private:
	void FillFromSmiColonString(wxArrayString &arr, const wxString &str);
	wxString ArrayToSmiColonString(const wxArrayString &array) const;
	void StripSemiColons(wxString &str);
	wxString NormalizePath(const wxString &path) const;

public:
	BuildConfig(wxXmlNode *node);
	virtual ~BuildConfig();
	wxXmlNode *ToXml() const;
	BuildConfig *Clone() const;

	//--------------------------------
	// Setters / Getters
	//--------------------------------
	wxString GetPreprocessor() const;
	void GetPreprocessor(wxArrayString &arr){ arr = m_preprocessor;}
	void SetPreprocessor(const wxString &prepr);
	
	const wxString &GetCompilerType() const { return m_compilerType; }
	void SetCompilerType(const wxString &cmpType) { m_compilerType = cmpType; }
	const wxString &GetDebuggerType() const { return m_debuggerType; }
	void SetDebuggerType(const wxString &type) { m_debuggerType = type; }

	wxString GetIncludePath() const;
	const wxString &GetCompileOptions() const { return m_compileOptions; }
	const wxString &GetLinkOptions() const { return m_linkOptions; }
	wxString GetLibraries() const;
	wxString GetLibPath() const;
	void GetPreBuildCommands(BuildCommandList &cmds) { cmds = m_preBuildCommands; }
	void GetPostBuildCommands(BuildCommandList &cmds) { cmds = m_postBuildCommands; }
	const wxString &GetName() const { return m_name; }
	bool IsCompilerRequired() const { return m_compilerRequired; }
	bool IsLinkerRequired() const { return m_linkerRequired; }
	wxString GetOutputFileName() const { return NormalizePath(m_outputFile); }
	wxString GetIntermediateDirectory() const { return NormalizePath(m_intermediateDirectory); }
	const wxString &GetCommand() const { return m_command; }
	const wxString &GetCommandArguments() const { return m_commandArguments;}
	wxString GetWorkingDirectory() const { return NormalizePath(m_workingDirectory);}
	bool IsCustomBuild() const {return m_enableCustomBuild;}
	const wxString &GetCustomBuildCmd()const{return m_customBuildCmd;}
	const wxString &GetCustomCleanCmd()const{return m_customCleanCmd;}
	void SetIncludePath(const wxArrayString &paths) { m_includePath = paths; }
	void SetIncludePath(const wxString &path);
	void SetLibraries(const wxString &libs);


	void SetLibPath(const wxString &path);
	void SetCompileOptions(const wxString &options) { m_compileOptions = options; }
	void SetLinkOptions(const wxString &options) { m_linkOptions = options; }
	void SetPreBuildCommands(const BuildCommandList &cmds) { m_preBuildCommands = cmds; }
	void SetPostBuildCommands(const BuildCommandList &cmds) { m_postBuildCommands = cmds; }
	void SetLibraries(const wxArrayString &libs) { m_libs = libs; }
	void SetLibPath(const wxArrayString &libPaths) { m_libPath = libPaths; }
	void SetName(const wxString &name){ m_name = name; }
	void SetCompilerRequired(bool required) { m_compilerRequired = required; }
	void SetLinkerRequired(bool required) { m_linkerRequired = required; }
	void SetOutputFileName(const wxString &name){ m_outputFile = name; }
	void SetIntermediateDirectory(const wxString &dir){ m_intermediateDirectory = dir; }
	void SetCommand(const wxString &cmd){ m_command = cmd; }
	void SetCommandArguments(const wxString &cmdArgs){ m_commandArguments = cmdArgs;}
	void SetWorkingDirectory(const wxString &dir){ m_workingDirectory = dir;}
	void SetCustomBuildCmd(const wxString &cmd){m_customBuildCmd = cmd;}
	void SetCustomCleanCmd(const wxString &cmd){m_customCleanCmd = cmd;}
	void EnableCustomBuild(bool enable){m_enableCustomBuild = enable;}


	void SetResCompilerRequired(bool required) { m_isResCmpNeeded = required; }
	bool IsResCompilerRequired() const { return m_isResCmpNeeded; }

	void SetResCmpIncludePath(const wxString &path){m_resCmpIncludePath = path;}
	const wxString& GetResCmpIncludePath() const{return m_resCmpIncludePath;}

	void SetResCmpOptions(const wxString &options){ m_resCompileOptions = options; }
	const wxString &GetResCompileOptions() const {return m_resCompileOptions;}

	//special custom rules
	wxString GetPreBuildCustom() const{return m_customPreBuildRule;}
	wxString GetPostBuildCustom() const{return m_customPostBuildRule;}

	void SetPreBuildCustom(const wxString& rule) {m_customPreBuildRule = rule;}
	void SetPostBuildCustom(const wxString& rule) {m_customPostBuildRule = rule;}
	
	void SetCustomBuildWorkingDir(const wxString& customBuildWorkingDir) {this->m_customBuildWorkingDir = customBuildWorkingDir;}
	const wxString& GetCustomBuildWorkingDir() const {return m_customBuildWorkingDir;}
	
	void SetPauseWhenExecEnds(const bool& pauseWhenExecEnds) {this->m_pauseWhenExecEnds = pauseWhenExecEnds;}
	const bool& GetPauseWhenExecEnds() const {return m_pauseWhenExecEnds;}
	
	void SetMakeGenerationCommand(const wxString& makeGenerationCommand) {this->m_makeGenerationCommand = makeGenerationCommand;}
	void SetToolName(const wxString& toolName) {this->m_toolName = toolName;}
	
	const wxString& GetMakeGenerationCommand() const {return m_makeGenerationCommand;}
	const wxString& GetToolName() const {return m_toolName;}
	
	void SetSingleFileBuildCommand(const wxString& singleFileBuildCommand) {this->m_singleFileBuildCommand = singleFileBuildCommand;}
	const wxString& GetSingleFileBuildCommand() const {return m_singleFileBuildCommand;}
};

typedef SmartPtr<BuildConfig> BuildConfigPtr;
#endif // BUILD_CONFIGURATION_H
