//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : compiler.cpp
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
#include "compiler.h"
#include "xmlutils.h"
#include "macros.h"
#include <wx/log.h>

Compiler::Compiler(wxXmlNode *node)
{
    // ensure all relevant entries exist in switches map (makes sure they show up in build settings dlg)
    m_switches[wxT("Include")] = wxEmptyString;
    m_switches[wxT("Debug")] = wxEmptyString;
    m_switches[wxT("Preprocessor")] = wxEmptyString;
    m_switches[wxT("Library")] = wxEmptyString;
    m_switches[wxT("LibraryPath")] = wxEmptyString;
    m_switches[wxT("Source")] = wxEmptyString;
    m_switches[wxT("Output")] = wxEmptyString;
    m_switches[wxT("Object")] = wxEmptyString;
    m_switches[wxT("ArchiveOutput")] = wxEmptyString;
    m_switches[wxT("PreprocessOnly")] = wxEmptyString;

    // ensure all relevant entries exist in tools map (makes sure they show up in build settings dlg)
    m_tools[wxT("LinkerName")] = wxEmptyString;
    m_tools[wxT("SharedObjectLinkerName")] = wxEmptyString;
    m_tools[wxT("CompilerName")] = wxEmptyString;
    m_tools[wxT("ArchiveTool")] = wxEmptyString;
    m_tools[wxT("ResourceCompiler")] = wxEmptyString;

	m_fileTypes.clear();
	if (node) {
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		if (!node->HasProp(wxT("GenerateDependenciesFiles"))) {
			if (m_name == wxT("gnu g++") || m_name == wxT("gnu gcc")) {
				m_generateDependeciesFile = true;
            } else
				m_generateDependeciesFile = false;
		} else {
			m_generateDependeciesFile = XmlUtils::ReadBool(node, wxT("GenerateDependenciesFiles"));
		}

		wxXmlNode *child = node->GetChildren();
		while (child) {
			if (child->GetName() == wxT("Switch")) {
				m_switches[XmlUtils::ReadString(child, wxT("Name"))] = XmlUtils::ReadString(child, wxT("Value"));
			}

			else if (child->GetName() == wxT("Tool")) {
				m_tools[XmlUtils::ReadString(child, wxT("Name"))] = XmlUtils::ReadString(child, wxT("Value"));
			}

			else if (child->GetName() == wxT("Option")) {
                wxString name = XmlUtils::ReadString(child, wxT("Name"));
				if (name == wxT("ObjectSuffix")) {
					m_objectSuffix = XmlUtils::ReadString(child, wxT("Value"));
				} else if (name == wxT("DependSuffix")) {
					m_dependSuffix = XmlUtils::ReadString(child, wxT("Value"));
				} else if (name == wxT("PreprocessSuffix")) {
					m_preprocessSuffix = XmlUtils::ReadString(child, wxT("Value"));
				}
			} else if (child->GetName() == wxT("File")) {
				Compiler::CmpFileTypeInfo ft;
				ft.compilation_line = XmlUtils::ReadString(child, wxT("CompilationLine"));
				ft.extension = XmlUtils::ReadString(child, wxT("Extension")).Lower();

				long kind = (long)CmpFileKindSource;
				if (XmlUtils::ReadLong(child, wxT("Kind"), kind) == CmpFileKindSource) {
					ft.kind = CmpFileKindSource;
				} else {
					ft.kind = CmpFileKindResource;
				}
				m_fileTypes[ft.extension] = ft;
			}

			else if (child->GetName() == wxT("Pattern")) {
				if (XmlUtils::ReadString(child, wxT("Name")) == wxT("Error")) {
					//found an error description
					CmpInfoPattern errPattern;
					errPattern.fileNameIndex = XmlUtils::ReadString(child, wxT("FileNameIndex"));
					errPattern.lineNumberIndex = XmlUtils::ReadString(child, wxT("LineNumberIndex"));
					errPattern.pattern = child->GetNodeContent();
					m_errorPatterns.push_back(errPattern);
				} else if (XmlUtils::ReadString(child, wxT("Name")) == wxT("Warning")) {
					//found a warning description
					CmpInfoPattern warnPattern;
					warnPattern.fileNameIndex = XmlUtils::ReadString(child, wxT("FileNameIndex"));
					warnPattern.lineNumberIndex = XmlUtils::ReadString(child, wxT("LineNumberIndex"));
					warnPattern.pattern = child->GetNodeContent();
					m_warningPatterns.push_back(warnPattern);
				}
			}

			else if (child->GetName() == wxT("GlobalIncludePath")) {
				m_globalIncludePath = child->GetNodeContent();
			}

			else if (child->GetName() == wxT("GlobalLibPath")) {
				m_globalLibPath = child->GetNodeContent();
			}

			else if (child->GetName() == wxT("PathVariable")) {
				m_pathVariable = child->GetNodeContent();
			}
			
			else if (child->GetName() == wxT("CompilerOption")) {
				CmpCmdLineOption cmpOption;
				cmpOption.name = XmlUtils::ReadString(child, wxT("Name"));
				cmpOption.help = child->GetNodeContent();
				m_compilerOptions[cmpOption.name] = cmpOption;
			}

			else if (child->GetName() == wxT("LinkerOption")) {
				CmpCmdLineOption cmpOption;
				cmpOption.name = XmlUtils::ReadString(child, wxT("Name"));
				cmpOption.help = child->GetNodeContent();
				m_linkerOptions[cmpOption.name] = cmpOption;
			}

			child = child->GetNext();
		}
	} else {
		//create a default compiler: g++
		m_name = wxT("gnu g++");
		m_switches[wxT("Include")] = wxT("-I");
		m_switches[wxT("Debug")] = wxT("-g ");
		m_switches[wxT("Preprocessor")] = wxT("-D");
		m_switches[wxT("Library")] = wxT("-l");
		m_switches[wxT("LibraryPath")] = wxT("-L");
		m_switches[wxT("Source")] = wxT("-c ");
		m_switches[wxT("Output")] = wxT("-o ");
		m_switches[wxT("Object")] = wxT("-o ");
		m_switches[wxT("ArchiveOutput")] = wxT(" ");
        m_switches[wxT("PreprocessOnly")] = wxT("-E");
		m_objectSuffix = wxT(".o");
		
		CmpInfoPattern pattern;
		pattern.pattern         = wxT("^([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]+ *)(:)( error)");
		pattern.fileNameIndex   = wxT("1");
		pattern.lineNumberIndex = wxT("3");
		m_errorPatterns.push_back(pattern);
		pattern.pattern         = wxT("^([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]+ *)(:)([0-9:]*)?( warning:)");
		pattern.fileNameIndex   = wxT("1");
		pattern.lineNumberIndex = wxT("3");
		m_warningPatterns.push_back(pattern);
		
		m_tools[wxT("LinkerName")] = wxT("g++");
		m_tools[wxT("SharedObjectLinkerName")] = wxT("g++ -shared -fPIC");
		m_tools[wxT("CompilerName")] = wxT("g++");
		m_tools[wxT("ArchiveTool")] = wxT("ar rcu");
		m_tools[wxT("ResourceCompiler")] = wxT("windres");
		m_globalIncludePath = wxEmptyString;
		m_globalLibPath = wxEmptyString;
		m_pathVariable = wxEmptyString;
		m_generateDependeciesFile = false;
	}
    if (m_generateDependeciesFile && m_dependSuffix.IsEmpty()) {
        m_dependSuffix = m_objectSuffix + wxT(".d");
    }
    if (!m_switches[wxT("PreprocessOnly")].IsEmpty() && m_preprocessSuffix.IsEmpty()) {
        m_preprocessSuffix = m_objectSuffix + wxT(".i");
    }

	if (m_fileTypes.empty()) {
		AddCmpFileType(wxT("cpp"), CmpFileKindSource, wxT("$(CompilerName) $(SourceSwitch) \"$(FileFullPath)\" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/$(FileName)$(ObjectSuffix) $(IncludePath)"));
		AddCmpFileType(wxT("cxx"), CmpFileKindSource, wxT("$(CompilerName) $(SourceSwitch) \"$(FileFullPath)\" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/$(FileName)$(ObjectSuffix) $(IncludePath)"));
		AddCmpFileType(wxT("c++"), CmpFileKindSource, wxT("$(CompilerName) $(SourceSwitch) \"$(FileFullPath)\" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/$(FileName)$(ObjectSuffix) $(IncludePath)"));
		AddCmpFileType(wxT("c"), CmpFileKindSource, wxT("$(CompilerName) $(SourceSwitch) \"$(FileFullPath)\" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/$(FileName)$(ObjectSuffix) $(IncludePath)"));
		AddCmpFileType(wxT("cc"), CmpFileKindSource, wxT("$(CompilerName) $(SourceSwitch) \"$(FileFullPath)\" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/$(FileName)$(ObjectSuffix) $(IncludePath)"));
		AddCmpFileType(wxT("m"), CmpFileKindSource, wxT("$(CompilerName) -X objective-c $(SourceSwitch) \"$(FileFullPath)\" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/$(FileName)$(ObjectSuffix) $(IncludePath)"));
		AddCmpFileType(wxT("mm"), CmpFileKindSource, wxT("$(CompilerName) -X objective-c++ $(SourceSwitch) \"$(FileFullPath)\" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/$(FileName)$(ObjectSuffix) $(IncludePath)"));
		AddCmpFileType(wxT("rc"), CmpFileKindResource, wxT("$(RcCompilerName) -i \"$(FileFullPath)\" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/$(FileFullName)$(ObjectSuffix) $(RcIncludePath)"));
	}
}

Compiler::~Compiler()
{
}

wxXmlNode *Compiler::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compiler"));
	node->AddProperty(wxT("Name"), m_name);
	node->AddProperty(wxT("GenerateDependenciesFiles"), BoolToString(m_generateDependeciesFile));

	std::map<wxString, wxString>::const_iterator iter = m_switches.begin();
	for (; iter != m_switches.end(); iter++) {
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Switch"));
		child->AddProperty(wxT("Name"), iter->first);
		child->AddProperty(wxT("Value"), iter->second);
		node->AddChild(child);
	}

	iter = m_tools.begin();
	for (; iter != m_tools.end(); iter++) {
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Tool"));
		child->AddProperty(wxT("Name"), iter->first);
		child->AddProperty(wxT("Value"), iter->second);
		node->AddChild(child);
	}

	std::map<wxString, Compiler::CmpFileTypeInfo>::const_iterator it = m_fileTypes.begin();
	for (; it != m_fileTypes.end(); it++) {
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
		Compiler::CmpFileTypeInfo ft = it->second;
		child->AddProperty(wxT("Extension"), ft.extension);
		child->AddProperty(wxT("CompilationLine"), ft.compilation_line);

		wxString strKind;
		strKind << (long)ft.kind;
		child->AddProperty(wxT("Kind"), strKind);

		node->AddChild(child);
	}

	wxXmlNode *options = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
	options->AddProperty(wxT("Name"),  wxT("ObjectSuffix"));
	options->AddProperty(wxT("Value"), m_objectSuffix);
	node->AddChild(options);

    options = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
    options->AddProperty(wxT("Name"),  wxT("DependSuffix"));
    options->AddProperty(wxT("Value"), m_dependSuffix);
	node->AddChild(options);

    options = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
    options->AddProperty(wxT("Name"),  wxT("PreprocessSuffix"));
    options->AddProperty(wxT("Value"), m_preprocessSuffix);
	node->AddChild(options);

	//add patterns
	CmpListInfoPattern::const_iterator itPattern;
	for (itPattern = m_errorPatterns.begin(); itPattern != m_errorPatterns.end(); ++itPattern) {
		wxXmlNode *error = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Pattern"));
		error->AddProperty(wxT("Name"), wxT("Error"));
		error->AddProperty(wxT("FileNameIndex"), itPattern->fileNameIndex);
		error->AddProperty(wxT("LineNumberIndex"), itPattern->lineNumberIndex);
		XmlUtils::SetNodeContent(error, itPattern->pattern);
		node->AddChild(error);
	}

	for (itPattern = m_warningPatterns.begin(); itPattern != m_warningPatterns.end(); ++itPattern) {
		wxXmlNode *warning = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Pattern"));
		warning->AddProperty(wxT("Name"), wxT("Warning"));
		warning->AddProperty(wxT("FileNameIndex"), itPattern->fileNameIndex);
		warning->AddProperty(wxT("LineNumberIndex"), itPattern->lineNumberIndex);
		XmlUtils::SetNodeContent(warning, itPattern->pattern);
		node->AddChild(warning);
	}

	wxXmlNode *globalIncludePath = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("GlobalIncludePath"));
	XmlUtils::SetNodeContent(globalIncludePath, m_globalIncludePath);
	node->AddChild(globalIncludePath);

	wxXmlNode *globalLibPath = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("GlobalLibPath"));
	XmlUtils::SetNodeContent(globalLibPath, m_globalLibPath);
	node->AddChild(globalLibPath);

	wxXmlNode *pathVariable = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PathVariable"));
	XmlUtils::SetNodeContent(pathVariable, m_pathVariable);
	node->AddChild(pathVariable);

	// Add compiler options
	CmpCmdLineOptions::const_iterator itCmpOption = m_compilerOptions.begin();
	for ( ; itCmpOption != m_compilerOptions.end(); ++itCmpOption)
	{
		const CmpCmdLineOption& cmpOption = itCmpOption->second;
		wxXmlNode* pCmpOptionNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CompilerOption"));
		pCmpOptionNode->AddProperty(wxT("Name"), cmpOption.name);
		XmlUtils::SetNodeContent(pCmpOptionNode, cmpOption.help);
		node->AddChild(pCmpOptionNode);
	}

	// Add linker options
	CmpCmdLineOptions::const_iterator itLnkOption = m_linkerOptions.begin();
	for ( ; itLnkOption != m_linkerOptions.end(); ++itLnkOption)
	{
		const CmpCmdLineOption& lnkOption = itLnkOption->second;
		wxXmlNode* pLnkOptionNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("LinkerOption"));
		pLnkOptionNode->AddProperty(wxT("Name"), lnkOption.name);
		XmlUtils::SetNodeContent(pLnkOptionNode, lnkOption.help);
		node->AddChild(pLnkOptionNode);
	}

	return node;
}

wxString Compiler::GetSwitch(const wxString &name) const
{
	std::map<wxString, wxString>::const_iterator iter = m_switches.find(name);
	if (iter == m_switches.end()) {
		return wxEmptyString;
	}
	return iter->second;
}

wxString Compiler::GetTool(const wxString &name) const
{
	std::map<wxString, wxString>::const_iterator iter = m_tools.find(name);
	if (iter == m_tools.end()) {
		return wxEmptyString;
	}
	return iter->second;
}

bool Compiler::GetCmpFileType(const wxString& extension, Compiler::CmpFileTypeInfo &ft)
{
	std::map<wxString, Compiler::CmpFileTypeInfo>::iterator iter = m_fileTypes.find(extension.Lower());
	if (iter == m_fileTypes.end()) {
		return false;
	}
	ft = iter->second;
	return true;
}

void Compiler::AddCmpFileType(const wxString &extension, CmpFileKind type, const wxString &compile_line)
{
	Compiler::CmpFileTypeInfo ft;
	ft.extension = extension.Lower();
	ft.compilation_line = compile_line;
	ft.kind = type;
	m_fileTypes[extension] = ft;
}
