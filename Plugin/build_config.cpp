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

BuildConfig::BuildConfig(wxXmlNode *node)
{
	if ( node ) {
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		m_compilerType = XmlUtils::ReadString(node, wxT("CompilerType"));
		m_debuggerType = XmlUtils::ReadString(node, wxT("DebuggerType"));
		wxXmlNode *compile = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
		m_projectType = XmlUtils::ReadString(node, wxT("Type"));

		// read the compile options
		if (compile) {
			m_compilerRequired = XmlUtils::ReadBool(compile, wxT("Required"), true);
			m_compileOptions = XmlUtils::ReadString(compile, wxT("Options"));
			wxXmlNode *child = compile->GetChildren();
			while (child) {
				if (child->GetName() == wxT("IncludePath")) {
					m_includePath.Add(XmlUtils::ReadString(child, wxT("Value")));
				} else if (child->GetName() == wxT("Preprocessor")) {
					m_preprocessor.Add(XmlUtils::ReadString(child, wxT("Value")));
				}
				child = child->GetNext();
			}
		}

		wxXmlNode *linker = XmlUtils::FindFirstByTagName(node, wxT("Linker"));
		// read the linker options
		if (linker) {
			m_linkerRequired = XmlUtils::ReadBool(linker, wxT("Required"), true);
			m_linkOptions = XmlUtils::ReadString(linker, wxT("Options"));
			wxXmlNode *child = linker->GetChildren();
			while (child) {
				if (child->GetName() == wxT("Library")) {
					m_libs.Add(XmlUtils::ReadString(child, wxT("Value")));
				} else if (child->GetName() == wxT("LibraryPath")) {
					m_libPath.Add(XmlUtils::ReadString(child, wxT("Value")));
				}
				child = child->GetNext();
			}
		}

		// read the postbuild commands
		wxXmlNode *debugger = XmlUtils::FindFirstByTagName(node, wxT("Debugger"));
		m_isDbgRemoteTarget = false;
		
		if (debugger) {
			m_isDbgRemoteTarget = XmlUtils::ReadBool(debugger, wxT("IsRemote"));
			m_dbgHostName = XmlUtils::ReadString(debugger, wxT("RemoteHostName"));
			m_dbgHostPort = XmlUtils::ReadString(debugger, wxT("RemoteHostPort"));
			m_debuggerStartupCmds = debugger->GetNodeContent();
		}

		// read the resource compile options
		wxXmlNode *resCmp = XmlUtils::FindFirstByTagName(node, wxT("ResourceCompiler"));
		if (resCmp) {
			m_isResCmpNeeded = XmlUtils::ReadBool(resCmp, wxT("Required"), true);
			m_resCompileOptions = XmlUtils::ReadString(resCmp, wxT("Options"));
			wxXmlNode *child = resCmp->GetChildren();
			while (child) {
				if (child->GetName() == wxT("IncludePath")) {
					m_resCmpIncludePath << XmlUtils::ReadString(child, wxT("Value")) << wxT(";");
				}
				child = child->GetNext();
			}
		}

		// read the prebuild commands
		wxXmlNode *preBuild = XmlUtils::FindFirstByTagName(node, wxT("PreBuild"));
		if (preBuild) {
			wxXmlNode *child = preBuild->GetChildren();
			while (child) {
				if (child->GetName() == wxT("Command")) {
					bool enabled = XmlUtils::ReadBool(child, wxT("Enabled"));

					BuildCommand cmd(child->GetNodeContent(), enabled);
					m_preBuildCommands.push_back(cmd);
				}
				child = child->GetNext();
			}
		}
		// read the postbuild commands
		wxXmlNode *postBuild = XmlUtils::FindFirstByTagName(node, wxT("PostBuild"));
		if (postBuild) {
			wxXmlNode *child = postBuild->GetChildren();
			while (child) {
				if (child->GetName() == wxT("Command")) {
					bool enabled = XmlUtils::ReadBool(child, wxT("Enabled"));
					BuildCommand cmd(child->GetNodeContent(), enabled);
					m_postBuildCommands.push_back(cmd);
				}
				child = child->GetNext();
			}
		}

		wxXmlNode *customBuild = XmlUtils::FindFirstByTagName(node, wxT("CustomBuild"));
		if (customBuild) {
			m_enableCustomBuild = XmlUtils::ReadBool(customBuild, wxT("Enabled"), false);
			wxXmlNode *child = customBuild->GetChildren();
			while (child) {
				if (child->GetName() == wxT("BuildCommand")) {
					m_customBuildCmd = child->GetNodeContent();
				} else if (child->GetName() == wxT("CleanCommand")) {
					m_customCleanCmd = child->GetNodeContent();
				} else if (child->GetName() == wxT("SingleFileCommand")) {
					m_singleFileBuildCommand = child->GetNodeContent();
				} else if (child->GetName() == wxT("WorkingDirectory")) {
					m_customBuildWorkingDir = child->GetNodeContent();
				} else if (child->GetName() == wxT("ThirdPartyToolName")) {
					m_toolName = child->GetNodeContent();
				} else if (child->GetName() == wxT("MakefileGenerationCommand")) {
					m_makeGenerationCommand = child->GetNodeContent();
				}
				child = child->GetNext();
			}
		} else {
			m_enableCustomBuild = false;
		}

		wxXmlNode *customPreBuild = XmlUtils::FindFirstByTagName(node, wxT("AdditionalRules"));
		if (customPreBuild) {
			wxXmlNode *child = customPreBuild->GetChildren();
			while (child) {
				if (child->GetName() == wxT("CustomPreBuild")) {
					m_customPreBuildRule = child->GetNodeContent();
				} else if (child->GetName() == wxT("CustomPostBuild")) {
					m_customPostBuildRule = child->GetNodeContent();
				}
				child = child->GetNext();
			}
		}

		wxXmlNode *general = XmlUtils::FindFirstByTagName(node, wxT("General"));
		if (general) {
			m_outputFile = XmlUtils::ReadString(general, wxT("OutputFile"));
			m_intermediateDirectory = XmlUtils::ReadString(general, wxT("IntermediateDirectory"), wxT("."));
			m_command = XmlUtils::ReadString(general, wxT("Command"));
			m_commandArguments = XmlUtils::ReadString(general, wxT("CommandArguments"));
			m_workingDirectory = XmlUtils::ReadString(general, wxT("WorkingDirectory"), wxT("."));
			m_pauseWhenExecEnds = XmlUtils::ReadBool(general, wxT("PauseExecWhenProcTerminates"), true);
		}
	} else {

		//create default project settings
		m_name = wxT("Debug");
		m_compilerRequired = true;
		m_includePath.Add(wxT("."));
		m_compileOptions = wxT("-g");
		m_linkOptions = wxT("-O0");
		m_libPath.Add(wxT("."));
		m_libPath.Add(wxT("Debug"));
		m_linkerRequired = true;
		m_intermediateDirectory = wxT("./Debug");
		m_workingDirectory = wxT("./Debug");
		m_projectType = Project::EXECUTABLE;
		m_enableCustomBuild = false;
		m_customBuildCmd = wxEmptyString;
		m_customCleanCmd = wxEmptyString;
		m_isResCmpNeeded = false;
		m_resCmpIncludePath = wxEmptyString;
		m_resCompileOptions = wxEmptyString;
		m_customPostBuildRule = wxEmptyString;
		m_customPreBuildRule = wxEmptyString;
		m_makeGenerationCommand = wxEmptyString;
		m_toolName = wxEmptyString;
		m_singleFileBuildCommand = wxEmptyString;
		m_debuggerStartupCmds = wxEmptyString;
		m_isDbgRemoteTarget = false;

		BuildSettingsConfigCookie cookie;
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
		if (cmp) {
			m_compilerType = cmp->GetName();
		}
		wxArrayString dbgs = DebuggerMgr::Get().GetAvailableDebuggers();
		if (dbgs.GetCount() > 0) {
			m_debuggerType = dbgs.Item(0);
		}
	}
}

BuildConfig::~BuildConfig()
{
}

wxString BuildConfig::NormalizePath(const wxString &path) const
{
	wxString normalized_path(path);
	normalized_path.Replace(wxT("\\"), wxT("/"));
	return normalized_path;
}

BuildConfig *BuildConfig::Clone() const
{
	wxXmlNode *node = ToXml();
	BuildConfig *cloned = new BuildConfig(node);
	delete node;
	return cloned;
}

wxXmlNode *BuildConfig::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Configuration"));
	node->AddProperty(wxT("Name"), m_name);
	node->AddProperty(wxT("CompilerType"), m_compilerType);
	node->AddProperty(wxT("DebuggerType"), m_debuggerType);
	node->AddProperty(wxT("Type"), m_projectType);

	wxXmlNode *general = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("General"));
	general->AddProperty(wxT("OutputFile"), m_outputFile);
	general->AddProperty(wxT("IntermediateDirectory"), m_intermediateDirectory);
	general->AddProperty(wxT("Command"), m_command );
	general->AddProperty(wxT("CommandArguments"), m_commandArguments);
	general->AddProperty(wxT("WorkingDirectory"), m_workingDirectory);
	general->AddProperty(wxT("PauseExecWhenProcTerminates"), BoolToString(m_pauseWhenExecEnds));
	node->AddChild(general);

	//create the compile node
	wxXmlNode *compile = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compiler"));
	compile->AddProperty(wxT("Required"), BoolToString(m_compilerRequired));
	compile->AddProperty(wxT("Options"), m_compileOptions);
	node->AddChild(compile);

	size_t i=0;
	for (i=0; i<m_includePath.GetCount(); i++) {
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("IncludePath"));
		option->AddProperty(wxT("Value"), m_includePath.Item(i));
		compile->AddChild(option);
	}

	for (i=0; i<m_preprocessor.GetCount(); i++) {
		wxXmlNode *prep = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Preprocessor"));
		prep->AddProperty(wxT("Value"), m_preprocessor.Item(i));
		compile->AddChild(prep);
	}

	//add the link node
	wxXmlNode *link = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Linker"));
	link->AddProperty(wxT("Required"), BoolToString(m_linkerRequired));
	link->AddProperty(wxT("Options"), m_linkOptions);
	node->AddChild(link);

	for (i=0; i<m_libPath.GetCount(); i++) {
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("LibraryPath"));
		option->AddProperty(wxT("Value"), m_libPath.Item(i));
		link->AddChild(option);
	}

	for (i=0; i<m_libs.GetCount(); i++) {
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Library"));
		option->AddProperty(wxT("Value"), m_libs.Item(i));
		link->AddChild(option);
	}

	wxXmlNode *debugger = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Debugger"));
	debugger->AddProperty(wxT("IsRemote"), BoolToString(m_isDbgRemoteTarget));
	debugger->AddProperty(wxT("RemoteHostName"), m_dbgHostName);
	debugger->AddProperty(wxT("RemoteHostPort"), m_dbgHostPort);
	
	XmlUtils::SetNodeContent(debugger, m_debuggerStartupCmds);
	node->AddChild(debugger);

	//add the resource compiler node
	wxXmlNode *resCmp = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ResourceCompiler"));
	resCmp->AddProperty(wxT("Required"), BoolToString(m_isResCmpNeeded));
	resCmp->AddProperty(wxT("Options"), m_resCompileOptions);
	node->AddChild(resCmp);

	wxStringTokenizer tok(m_resCmpIncludePath, wxT(";"));
	while (tok.HasMoreTokens()) {
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("IncludePath"));
		option->AddProperty(wxT("Value"),tok.NextToken());
		resCmp->AddChild(option);
	}

	//add prebuild commands
	wxXmlNode *preBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PreBuild"));
	node->AddChild(preBuild);

	BuildCommandList::const_iterator iter = m_preBuildCommands.begin();
	for (; iter != m_preBuildCommands.end(); iter++) {
		wxXmlNode *command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
		command->AddProperty(wxT("Enabled"), BoolToString(iter->GetEnabled()));
		XmlUtils::SetNodeContent(command, iter->GetCommand());
		preBuild->AddChild(command);
	}

	//add postbuild commands
	wxXmlNode *postBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PostBuild"));
	node->AddChild(postBuild);
	iter = m_postBuildCommands.begin();
	for (; iter != m_postBuildCommands.end(); iter++) {
		wxXmlNode *command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
		command->AddProperty(wxT("Enabled"), BoolToString(iter->GetEnabled()));
		XmlUtils::SetNodeContent(command, iter->GetCommand());
		postBuild->AddChild(command);
	}

	//add postbuild commands
	wxXmlNode *customBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CustomBuild"));
	node->AddChild(customBuild);
	customBuild->AddProperty(wxT("Enabled"), BoolToString(m_enableCustomBuild));

	//add the working directory of the custom build
	wxXmlNode *customBuildWd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("WorkingDirectory"));
	XmlUtils::SetNodeContent(customBuildWd, m_customBuildWorkingDir);

	//add the makefile generation command
	wxXmlNode *toolName = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("ThirdPartyToolName"));
	XmlUtils::SetNodeContent(toolName, m_toolName);

	//add the makefile generation command
	wxXmlNode *makeGenCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("MakefileGenerationCommand"));
	XmlUtils::SetNodeContent(makeGenCmd, m_makeGenerationCommand);

	//add the makefile generation command
	wxXmlNode *singleFileCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("SingleFileCommand"));
	XmlUtils::SetNodeContent(singleFileCmd, m_singleFileBuildCommand);

	//add build and clean commands
	wxXmlNode *bldCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("BuildCommand"));
	XmlUtils::SetNodeContent(bldCmd, m_customBuildCmd);

	wxXmlNode *clnCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("CleanCommand"));
	XmlUtils::SetNodeContent(clnCmd, m_customCleanCmd);

	//add the additional rules
	wxXmlNode *additionalCmds = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("AdditionalRules"));
	node->AddChild(additionalCmds);

	wxXmlNode *preCmd = new wxXmlNode(additionalCmds, wxXML_ELEMENT_NODE, wxT("CustomPreBuild"));
	XmlUtils::SetNodeContent(preCmd, m_customPreBuildRule);
	wxXmlNode *postCmd = new wxXmlNode(additionalCmds, wxXML_ELEMENT_NODE, wxT("CustomPostBuild"));
	XmlUtils::SetNodeContent(postCmd, m_customPostBuildRule);
	return node;
}

void BuildConfig::SetPreprocessor(const wxString &pre)
{
	FillFromSmiColonString(m_preprocessor, pre);
}

void BuildConfig::SetIncludePath(const wxString &path)
{
	FillFromSmiColonString(m_includePath, path);
}

void BuildConfig::SetLibraries(const wxString &libs)
{
	FillFromSmiColonString(m_libs, libs);
}

void BuildConfig::SetLibPath(const wxString &paths)
{
	FillFromSmiColonString(m_libPath, paths);
}

void BuildConfig::FillFromSmiColonString(wxArrayString &arr, const wxString &str)
{
	arr.clear();
	wxStringTokenizer tkz(str, wxT(";"));
	while (tkz.HasMoreTokens()) {
		wxString token = tkz.NextToken();
		arr.Add(token.Trim());
	}
}
// Utils function
wxString BuildConfig::ArrayToSmiColonString(const wxArrayString &array) const
{
	wxString result;
	for (size_t i=0; i<array.GetCount(); i++) {
		result += NormalizePath(array.Item(i));
		result += wxT(";");
	}
	return result.BeforeLast(wxT(';'));
}

void BuildConfig::StripSemiColons(wxString &str)
{
	str.Replace(wxT(";"), wxT(" "));
}

wxString BuildConfig::GetLibPath() const
{
	return ArrayToSmiColonString(m_libPath);
}

wxString BuildConfig::GetLibraries() const
{
	return ArrayToSmiColonString(m_libs);
}

wxString BuildConfig::GetIncludePath() const
{
	return ArrayToSmiColonString(m_includePath);
}

wxString BuildConfig::GetPreprocessor() const
{
	return ArrayToSmiColonString(m_preprocessor);
}
