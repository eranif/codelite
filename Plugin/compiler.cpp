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
#include "CxxPreProcessor.h"
#include "asyncprocess.h"
#include "build_settings_config.h"
#include "build_system.h"
#include "compiler.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "macros.h"
#include "wx_xml_compatibility.h"
#include "xmlutils.h"
#include <ICompilerLocator.h>
#include <procutils.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>

Compiler::Compiler(wxXmlNode* node, Compiler::eRegexType regexType)
    : m_objectNameIdenticalToFileName(false)
    , m_isDefault(false)
{
    // ensure all relevant entries exist in switches map (makes sure they show up in build settings dlg)
    SetSwitch("Include", "");
    SetSwitch("Debug", "");
    SetSwitch("Preprocessor", "");
    SetSwitch("Library", "");
    SetSwitch("LibraryPath", "");
    SetSwitch("Source", "");
    SetSwitch("Output", "");
    SetSwitch("Object", "");
    SetSwitch("ArchiveOutput", "");
    SetSwitch("PreprocessOnly", "");

    // ensure all relevant entries exist in tools map (makes sure they show up in build settings dlg)
    SetTool("LinkerName", "");
    SetTool("SharedObjectLinkerName", "");
    SetTool("CXX", "");
    SetTool("CC", "");
    SetTool("AR", "");
    SetTool("ResourceCompiler", "");
    SetTool("MAKE", "");
    SetTool("AS", "as"); // Assembler, default to as

    m_fileTypes.clear();
    if(node) {
        m_name = XmlUtils::ReadString(node, wxT("Name"));
        m_compilerFamily = XmlUtils::ReadString(node, "CompilerFamily");

        if(m_compilerFamily == "GNU GCC") {
            // fix wrong name
            m_compilerFamily = COMPILER_FAMILY_GCC;
        }

        m_isDefault = XmlUtils::ReadBool(node, "IsDefault");

        if(!node->HasProp(wxT("GenerateDependenciesFiles"))) {
            if(m_name == wxT("gnu g++") || m_name == wxT("gnu gcc")) {
                m_generateDependeciesFile = true;
            } else
                m_generateDependeciesFile = false;
        } else {
            m_generateDependeciesFile = XmlUtils::ReadBool(node, wxT("GenerateDependenciesFiles"));
        }

        if(!node->HasProp(wxT("ReadObjectsListFromFile"))) {
            m_readObjectFilesFromList = true;
        } else {
            m_readObjectFilesFromList = XmlUtils::ReadBool(node, wxT("ReadObjectsListFromFile"));
        }

        m_objectNameIdenticalToFileName = XmlUtils::ReadBool(node, wxT("ObjectNameIdenticalToFileName"));

        wxXmlNode* child = node->GetChildren();
        while(child) {
            if(child->GetName() == wxT("Switch")) {
                m_switches[XmlUtils::ReadString(child, wxT("Name"))] = XmlUtils::ReadString(child, wxT("Value"));
            }

            else if(child->GetName() == wxT("Tool")) {
                wxString toolpath = XmlUtils::ReadString(child, wxT("Value"));
                toolpath.Trim();
                m_tools[XmlUtils::ReadString(child, wxT("Name"))] = toolpath;
            }

            else if(child->GetName() == wxT("Option")) {
                wxString name = XmlUtils::ReadString(child, wxT("Name"));
                if(name == wxT("ObjectSuffix")) {
                    m_objectSuffix = XmlUtils::ReadString(child, wxT("Value"));
                } else if(name == wxT("DependSuffix")) {
                    m_dependSuffix = XmlUtils::ReadString(child, wxT("Value"));
                } else if(name == wxT("PreprocessSuffix")) {
                    m_preprocessSuffix = XmlUtils::ReadString(child, wxT("Value"));
                }
            } else if(child->GetName() == wxT("File")) {
                Compiler::CmpFileTypeInfo ft;
                ft.compilation_line = XmlUtils::ReadString(child, wxT("CompilationLine"));
                ft.extension = XmlUtils::ReadString(child, wxT("Extension")).Lower();

                long kind = (long)CmpFileKindSource;
                if(XmlUtils::ReadLong(child, wxT("Kind"), kind) == CmpFileKindSource) {
                    ft.kind = CmpFileKindSource;
                } else {
                    ft.kind = CmpFileKindResource;
                }
                m_fileTypes[ft.extension] = ft;
            } else if(child->GetName() == "LinkLine") {
                wxString type = XmlUtils::ReadString(child, "ProjectType");
                wxString line = XmlUtils::ReadString(child, "Pattern");
                wxString lineFromFile = XmlUtils::ReadString(child, "PatternWithFile");
                m_linkerLines.insert({ type, { lineFromFile, line } });

            } else if(child->GetName() == wxT("Pattern")) {
                if(XmlUtils::ReadString(child, wxT("Name")) == wxT("Error")) {
                    // found an error description
                    CmpInfoPattern errPattern;
                    errPattern.fileNameIndex = XmlUtils::ReadString(child, wxT("FileNameIndex"));
                    errPattern.lineNumberIndex = XmlUtils::ReadString(child, wxT("LineNumberIndex"));
                    errPattern.columnIndex = XmlUtils::ReadString(child, "ColumnIndex", "-1");
                    errPattern.pattern = child->GetNodeContent();
                    m_errorPatterns.push_back(errPattern);
                } else if(XmlUtils::ReadString(child, wxT("Name")) == wxT("Warning")) {
                    // found a warning description
                    CmpInfoPattern warnPattern;
                    warnPattern.fileNameIndex = XmlUtils::ReadString(child, wxT("FileNameIndex"));
                    warnPattern.lineNumberIndex = XmlUtils::ReadString(child, wxT("LineNumberIndex"));
                    warnPattern.columnIndex = XmlUtils::ReadString(child, "ColumnIndex", "-1");
                    warnPattern.pattern = child->GetNodeContent();
                    m_warningPatterns.push_back(warnPattern);
                }
            }

            else if(child->GetName() == wxT("GlobalIncludePath")) {
                m_globalIncludePath = child->GetNodeContent();
            }

            else if(child->GetName() == wxT("GlobalLibPath")) {
                m_globalLibPath = child->GetNodeContent();
            }

            else if(child->GetName() == wxT("PathVariable")) {
                m_pathVariable = child->GetNodeContent();
            }

            else if(child->GetName() == wxT("CompilerOption")) {
                CmpCmdLineOption cmpOption;
                cmpOption.name = XmlUtils::ReadString(child, wxT("Name"));
                cmpOption.help = child->GetNodeContent();
                m_compilerOptions[cmpOption.name] = cmpOption;
            }

            else if(child->GetName() == wxT("LinkerOption")) {
                CmpCmdLineOption cmpOption;
                cmpOption.name = XmlUtils::ReadString(child, wxT("Name"));
                cmpOption.help = child->GetNodeContent();
                m_linkerOptions[cmpOption.name] = cmpOption;
            }

            else if(child->GetName() == wxT("InstallationPath")) {
                m_installationPath = child->GetNodeContent();
            }

            child = child->GetNext();
        }

        if(GetTool("MAKE").IsEmpty()) {
            BuilderConfigPtr bldr = BuildSettingsConfigST::Get()->GetBuilderConfig("");
            if(bldr) { SetTool("MAKE", wxString() << bldr->GetToolPath() << " -j " << bldr->GetToolJobs()); }
        }

        // Default values for the assembler
        if(GetTool("AS").IsEmpty() && GetName().CmpNoCase("vc++") == 0) {
            SetTool("AS", "nasm");

        } else if(GetTool("AS").IsEmpty()) {
            SetTool("AS", "as");
        }

        // For backward compatibility, if the compiler / linker options are empty - add them
        if(IsGnuCompatibleCompiler()) { AddDefaultGnuComplierOptions(); }

        if(IsGnuCompatibleCompiler()) { AddDefaultGnuLinkerOptions(); }

    } else {
        // Create a default compiler: g++
        m_name = "";
        m_compilerFamily = COMPILER_FAMILY_GCC;
        m_isDefault = false;
        SetSwitch("Include", "-I");
        SetSwitch("Debug", "-g ");
        SetSwitch("Preprocessor", "-D");
        SetSwitch("Library", "-l");
        SetSwitch("LibraryPath", "-L");
        SetSwitch("Source", "-c ");
        SetSwitch("Output", "-o ");
        SetSwitch("Object", "-o ");
        SetSwitch("ArchiveOutput", " ");
        SetSwitch("PreprocessOnly", "-E");

        m_objectSuffix = ".o";
        m_preprocessSuffix = ".i";

        if(regexType == kRegexGNU) {
            AddPattern(eErrorPattern,
                       "^([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]*)([:0-9]*)(: )((fatal "
                       "error)|(error)|(undefined reference)|([\\t ]*required from))",
                       1, 3, 4);
            AddPattern(eErrorPattern,
                       "^([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ "
                       "*)(:)(\\(\\.text\\+[0-9a-fx]*\\))",
                       3, 1, -1);
            AddPattern(eErrorPattern,
                       "^([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([^ ][a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ "
                       "*)(:)([0-9]+)(:)",
                       3, 1, -1);
            AddPattern(eErrorPattern, "undefined reference to", -1, -1, -1);
            AddPattern(eErrorPattern, "\\*\\*\\* \\[[a-zA-Z\\-_0-9 ]+\\] (Error)", -1, -1, -1);

            AddPattern(eWarningPattern,
                       "([a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]+ *)(:)([0-9:]*)?[ \\t]*(warning|required)",
                       1, 3, 4);
            AddPattern(eWarningPattern, "([a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]+ *)(:)([0-9:]*)?( note)",
                       1, 3, -1);
            AddPattern(eWarningPattern,
                       "([a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]+ *)(:)([0-9:]*)?([ ]+instantiated)", 1, 3,
                       -1);
            AddPattern(eWarningPattern,
                       "(In file included from *)([a-zA-Z:]{0,2}[ a-zA-Z\\.0-9_/\\+\\-]+ *)(:)([0-9]+ *)(:)([0-9:]*)?",
                       2, 4, -1);

            AddDefaultGnuComplierOptions();
            AddDefaultGnuLinkerOptions();

        } else {

            AddPattern(eErrorPattern, "^windres: ([a-zA-Z:]{0,2}[ a-zA-Z\\\\.0-9_/\\+\\-]+) *:([0-9]+): syntax error",
                       1, 2);
            AddPattern(eErrorPattern, "(^[a-zA-Z\\\\.0-9 _/\\:\\+\\-]+ *)(\\()([0-9]+)(\\))( \\: )(error)", 1, 3);
            AddPattern(eErrorPattern, "(LINK : fatal error)", 1, 1);
            AddPattern(eErrorPattern, "(NMAKE : fatal error)", 1, 1);
            AddPattern(eWarningPattern, "(^[a-zA-Z\\\\.0-9 _/\\:\\+\\-]+ *)(\\()([0-9]+)(\\))( \\: )(warning)", 1, 3);
            AddPattern(eWarningPattern, "([a-z_A-Z]*\\.obj)( : warning)", 1, 1);
            AddPattern(eWarningPattern, "(cl : Command line warning)", 1, 1);
        }

        SetTool("LinkerName", "g++");
#ifdef __WXMAC__
        SetTool("SharedObjectLinkerName", "g++ -dynamiclib -fPIC");
#else
        SetTool("SharedObjectLinkerName", "g++ -shared -fPIC");
#endif

        SetTool("CXX", "g++");
        SetTool("CC", "gcc");
        SetTool("AR", "ar rcu");
        SetTool("ResourceCompiler", "windres");
        SetTool("AS", "as");

#ifdef __WXMSW__
        SetTool("MAKE", "mingw32-make");
#else
        SetTool("MAKE", "make");
#endif

        m_globalIncludePath = wxEmptyString;
        m_globalLibPath = wxEmptyString;
        m_pathVariable = wxEmptyString;
        m_generateDependeciesFile = false;
        m_readObjectFilesFromList = true;
        m_objectNameIdenticalToFileName = false;
    }

    if(m_generateDependeciesFile && m_dependSuffix.IsEmpty()) { m_dependSuffix = m_objectSuffix + wxT(".d"); }

    if(!m_switches[wxT("PreprocessOnly")].IsEmpty() && m_preprocessSuffix.IsEmpty()) {
        m_preprocessSuffix = m_objectSuffix + wxT(".i");
    }

    if(m_fileTypes.empty()) {
        AddCmpFileType("cpp", CmpFileKindSource,
                       "$(CXX) $(SourceSwitch) \"$(FileFullPath)\" $(CXXFLAGS) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/$(ObjectName)$(ObjectSuffix) "
                       "$(IncludePath)");
        AddCmpFileType("cxx", CmpFileKindSource,
                       "$(CXX) $(SourceSwitch) \"$(FileFullPath)\" $(CXXFLAGS) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/$(ObjectName)$(ObjectSuffix) "
                       "$(IncludePath)");
        AddCmpFileType("c++", CmpFileKindSource,
                       "$(CXX) $(SourceSwitch) \"$(FileFullPath)\" $(CXXFLAGS) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/$(ObjectName)$(ObjectSuffix) "
                       "$(IncludePath)");
        AddCmpFileType("c", CmpFileKindSource,
                       "$(CC) $(SourceSwitch) \"$(FileFullPath)\" $(CFLAGS) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/$(ObjectName)$(ObjectSuffix) "
                       "$(IncludePath)");
        AddCmpFileType("cc", CmpFileKindSource,
                       "$(CXX) $(SourceSwitch) \"$(FileFullPath)\" $(CXXFLAGS) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/$(ObjectName)$(ObjectSuffix) "
                       "$(IncludePath)");
        AddCmpFileType("m", CmpFileKindSource,
                       "$(CXX) -x objective-c $(SourceSwitch) \"$(FileFullPath)\" $(CXXFLAGS) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/$(ObjectName)$(ObjectSuffix) "
                       "$(IncludePath)");
        AddCmpFileType("mm", CmpFileKindSource,
                       "$(CXX) -x objective-c++ $(SourceSwitch) \"$(FileFullPath)\" "
                       "$(CXXFLAGS) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/$(ObjectName)$(ObjectSuffix) "
                       "$(IncludePath)");
        AddCmpFileType("s", CmpFileKindSource,
                       "$(AS) \"$(FileFullPath)\" $(ASFLAGS) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/$(ObjectName)$(ObjectSuffix) "
                       "-I$(IncludePath)");
        AddCmpFileType("rc", CmpFileKindResource,
                       "$(RcCompilerName) -i \"$(FileFullPath)\" $(RcCmpOptions)   "
                       "$(ObjectSwitch)$(IntermediateDirectory)/"
                       "$(ObjectName)$(ObjectSuffix) $(RcIncludePath)");
    }

    // Set linker lines
    if(m_linkerLines.empty()) {
        m_linkerLines.insert({ PROJECT_TYPE_STATIC_LIBRARY,
                               { "$(AR) $(ArchiveOutputSwitch)$(OutputFile) @$(ObjectsFileList)",
                                 "$(AR) $(ArchiveOutputSwitch)$(OutputFile) $(Objects)" } });
        m_linkerLines.insert({ PROJECT_TYPE_DYNAMIC_LIBRARY,
                               { "$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) "
                                 "$(LibPath) $(Libs) $(LinkOptions)",
                                 "$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) "
                                 "$(LinkOptions)" } });
        m_linkerLines.insert(
            { PROJECT_TYPE_EXECUTABLE,
              { "$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)",
                "$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)" } });
    }

    // Add support for assembler file
    if(m_fileTypes.count("s") == 0) {
        AddCmpFileType("s", CmpFileKindSource,
                       "$(AS) \"$(FileFullPath)\" $(ASFLAGS) "
                       "$(ObjectSwitch)$(IntermediateDirectory)/$(ObjectName)$(ObjectSuffix) "
                       "-I$(IncludePath)");
    }
}

Compiler::~Compiler() {}

wxXmlNode* Compiler::ToXml() const
{
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compiler"));
    node->AddProperty(wxT("Name"), m_name);
    node->AddProperty(wxT("GenerateDependenciesFiles"), BoolToString(m_generateDependeciesFile));
    node->AddProperty(wxT("ReadObjectsListFromFile"), BoolToString(m_readObjectFilesFromList));
    node->AddProperty(wxT("ObjectNameIdenticalToFileName"), BoolToString(m_objectNameIdenticalToFileName));
    node->AddProperty("CompilerFamily", m_compilerFamily);
    node->AddProperty("IsDefault", BoolToString(m_isDefault));

    wxXmlNode* installPath = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "InstallationPath");
    node->AddChild(installPath);
    XmlUtils::SetCDATANodeContent(installPath, m_installationPath);

    std::map<wxString, wxString>::const_iterator iter = m_switches.begin();
    for(; iter != m_switches.end(); iter++) {
        wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Switch"));
        child->AddProperty(wxT("Name"), iter->first);
        child->AddProperty(wxT("Value"), iter->second);
        node->AddChild(child);
    }

    iter = m_tools.begin();
    for(; iter != m_tools.end(); iter++) {
        wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Tool"));
        child->AddProperty(wxT("Name"), iter->first);
        child->AddProperty(wxT("Value"), iter->second);
        node->AddChild(child);
    }

    std::map<wxString, Compiler::CmpFileTypeInfo>::const_iterator it = m_fileTypes.begin();
    for(; it != m_fileTypes.end(); it++) {
        wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
        Compiler::CmpFileTypeInfo ft = it->second;
        child->AddProperty(wxT("Extension"), ft.extension);
        child->AddProperty(wxT("CompilationLine"), ft.compilation_line);

        wxString strKind;
        strKind << (long)ft.kind;
        child->AddProperty(wxT("Kind"), strKind);

        node->AddChild(child);
    }

    std::map<wxString, LinkLine>::const_iterator it2 = m_linkerLines.begin();
    for(; it2 != m_linkerLines.end(); it2++) {
        wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "LinkLine");
        child->AddProperty("ProjectType", it2->first);
        child->AddProperty("Pattern", it2->second.line);
        child->AddProperty("PatternWithFile", it2->second.lineFromFile);
        node->AddChild(child);
    }

    wxXmlNode* options = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
    options->AddProperty(wxT("Name"), wxT("ObjectSuffix"));
    options->AddProperty(wxT("Value"), m_objectSuffix);
    node->AddChild(options);

    options = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
    options->AddProperty(wxT("Name"), wxT("DependSuffix"));
    options->AddProperty(wxT("Value"), m_dependSuffix);
    node->AddChild(options);

    options = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
    options->AddProperty(wxT("Name"), wxT("PreprocessSuffix"));
    options->AddProperty(wxT("Value"), m_preprocessSuffix);
    node->AddChild(options);

    // add patterns
    CmpListInfoPattern::const_iterator itPattern;
    for(itPattern = m_errorPatterns.begin(); itPattern != m_errorPatterns.end(); ++itPattern) {
        wxXmlNode* error = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Pattern"));
        error->AddAttribute(wxT("Name"), wxT("Error"));
        error->AddAttribute(wxT("FileNameIndex"), itPattern->fileNameIndex);
        error->AddAttribute(wxT("LineNumberIndex"), itPattern->lineNumberIndex);
        error->AddAttribute(wxT("ColumnIndex"), itPattern->columnIndex);
        XmlUtils::SetNodeContent(error, itPattern->pattern);
        node->AddChild(error);
    }

    for(itPattern = m_warningPatterns.begin(); itPattern != m_warningPatterns.end(); ++itPattern) {
        wxXmlNode* warning = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Pattern"));
        warning->AddAttribute(wxT("Name"), wxT("Warning"));
        warning->AddAttribute(wxT("FileNameIndex"), itPattern->fileNameIndex);
        warning->AddAttribute(wxT("LineNumberIndex"), itPattern->lineNumberIndex);
        warning->AddAttribute(wxT("ColumnIndex"), itPattern->columnIndex);
        XmlUtils::SetNodeContent(warning, itPattern->pattern);
        node->AddChild(warning);
    }

    wxXmlNode* globalIncludePath = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("GlobalIncludePath"));
    XmlUtils::SetNodeContent(globalIncludePath, m_globalIncludePath);
    node->AddChild(globalIncludePath);

    wxXmlNode* globalLibPath = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("GlobalLibPath"));
    XmlUtils::SetNodeContent(globalLibPath, m_globalLibPath);
    node->AddChild(globalLibPath);

    wxXmlNode* pathVariable = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PathVariable"));
    XmlUtils::SetNodeContent(pathVariable, m_pathVariable);
    node->AddChild(pathVariable);

    // Add compiler options
    CmpCmdLineOptions::const_iterator itCmpOption = m_compilerOptions.begin();
    for(; itCmpOption != m_compilerOptions.end(); ++itCmpOption) {
        const CmpCmdLineOption& cmpOption = itCmpOption->second;
        wxXmlNode* pCmpOptionNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CompilerOption"));
        pCmpOptionNode->AddProperty(wxT("Name"), cmpOption.name);
        XmlUtils::SetNodeContent(pCmpOptionNode, cmpOption.help);
        node->AddChild(pCmpOptionNode);
    }

    // Add linker options
    CmpCmdLineOptions::const_iterator itLnkOption = m_linkerOptions.begin();
    for(; itLnkOption != m_linkerOptions.end(); ++itLnkOption) {
        const CmpCmdLineOption& lnkOption = itLnkOption->second;
        wxXmlNode* pLnkOptionNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("LinkerOption"));
        pLnkOptionNode->AddProperty(wxT("Name"), lnkOption.name);
        XmlUtils::SetNodeContent(pLnkOptionNode, lnkOption.help);
        node->AddChild(pLnkOptionNode);
    }

    return node;
}

wxString Compiler::GetSwitch(const wxString& name) const
{
    std::map<wxString, wxString>::const_iterator iter = m_switches.find(name);
    if(iter == m_switches.end()) { return wxEmptyString; }
    return iter->second;
}

wxString Compiler::GetTool(const wxString& name) const
{
    std::map<wxString, wxString>::const_iterator iter = m_tools.find(name);
    if(iter == m_tools.end()) {
        if(name == wxT("CC")) {
            // an upgrade, return the CXX
            return GetTool(wxT("CXX"));
        }
        return wxEmptyString;
    }
    if(name == wxT("CC") && iter->second.empty()) { return GetTool(wxT("CXX")); }
    wxString tool = iter->second;
    tool.Replace("\\", "/");
    return tool;
}

bool Compiler::GetCmpFileType(const wxString& extension, Compiler::CmpFileTypeInfo& ft)
{
    std::map<wxString, Compiler::CmpFileTypeInfo>::iterator iter = m_fileTypes.find(extension.Lower());
    if(iter == m_fileTypes.end()) { return false; }
    ft = iter->second;
    return true;
}

void Compiler::AddCmpFileType(const wxString& extension, CmpFileKind type, const wxString& compile_line)
{
    Compiler::CmpFileTypeInfo ft;
    ft.extension = extension.Lower();
    if(m_fileTypes.count(ft.extension)) { m_fileTypes.erase(ft.extension); }

    ft.compilation_line = compile_line;
    ft.kind = type;
    m_fileTypes[extension] = ft;
}

void Compiler::SetSwitch(const wxString& switchName, const wxString& switchValue)
{
    if(m_switches.count(switchName)) { m_switches.erase(switchName); }
    m_switches.insert(std::make_pair(switchName, switchValue));
}

void Compiler::AddPattern(int type, const wxString& pattern, int fileNameIndex, int lineNumberIndex, int colIndex)
{
    CmpInfoPattern pt;
    pt.pattern = pattern;
    pt.fileNameIndex = wxString::Format("%d", (int)fileNameIndex);
    pt.lineNumberIndex = wxString::Format("%d", (int)lineNumberIndex);
    pt.columnIndex = wxString::Format("%d", colIndex);
    if(type == eErrorPattern) {
        m_errorPatterns.push_back(pt);

    } else {
        m_warningPatterns.push_back(pt);
    }
}

void Compiler::SetTool(const wxString& toolname, const wxString& cmd)
{
    if(m_tools.count(toolname)) { m_tools.erase(toolname); }
    m_tools.insert(std::make_pair(toolname, cmd));
}

void Compiler::AddCompilerOption(const wxString& name, const wxString& desc)
{
    CmpCmdLineOption option;
    option.help = desc;
    option.name = name;
    m_compilerOptions.erase(name);
    m_compilerOptions.insert(std::make_pair(name, option));
}

void Compiler::AddLinkerOption(const wxString& name, const wxString& desc)
{
    CmpCmdLineOption option;
    option.help = desc;
    option.name = name;
    m_linkerOptions.erase(name);
    m_linkerOptions.insert(std::make_pair(name, option));
}

bool Compiler::IsGnuCompatibleCompiler() const
{
    return m_compilerFamily.IsEmpty() || m_compilerFamily == COMPILER_FAMILY_CLANG ||
           m_compilerFamily == COMPILER_FAMILY_GCC || m_compilerFamily == COMPILER_FAMILY_MINGW;
}

void Compiler::AddDefaultGnuComplierOptions()
{
    // Add GCC / CLANG default compiler options
    AddCompilerOption("-O", "Optimize generated code. (for speed)");
    AddCompilerOption("-O1", "Optimize more (for speed)");
    AddCompilerOption("-O2", "Optimize even more (for speed)");
    AddCompilerOption("-O3", "Optimize fully (for speed)");
    AddCompilerOption("-Os", "Optimize generated code (for size)");
    AddCompilerOption("-O0", "Optimize for debugging");
    AddCompilerOption("-W", "Enable standard compiler warnings");
    AddCompilerOption("-Wall", "Enable all compiler warnings");
    AddCompilerOption("-Wfatal-errors", "Stop compiling after first error");
    AddCompilerOption("-Wmain", "Warn if main() is not conformant");
    AddCompilerOption(
        "-ansi",
        "In C mode, support all ISO C90 programs. In C++ mode, remove GNU extensions that conflict with ISO C++");
    AddCompilerOption("-fexpensive-optimizations", "Expensive optimizations");
    AddCompilerOption("-fopenmp", "Enable OpenMP (compilation)");
    AddCompilerOption("-g", "Produce debugging information");
    AddCompilerOption("-pedantic", "Enable warnings demanded by strict ISO C and ISO C++");
    AddCompilerOption("-pedantic-errors", "Treat as errors the warnings demanded by strict ISO C and ISO C++");
    AddCompilerOption("-pg", "Profile code when executed");
    AddCompilerOption("-w", "Inhibit all warning messages");
    AddCompilerOption("-std=c99", "Enable ANSI C99 features");
    AddCompilerOption("-std=c++11", "Enable C++11 features");
    AddCompilerOption("-std=c++14", "Enable C++14 features");
    AddCompilerOption("-std=c++17", "Enable C++17 features");
}

void Compiler::AddDefaultGnuLinkerOptions()
{
    // Linker options
    AddLinkerOption("-fopenmp", "Enable OpenMP (linkage)");
    AddLinkerOption("-mwindows", "Prevent a useless terminal console appearing with MSWindows GUI programs");
    AddLinkerOption("-pg", "Profile code when executed");
    AddLinkerOption("-s", "Remove all symbol table and relocation information from the executable");
}

wxArrayString Compiler::GetDefaultIncludePaths()
{
    wxArrayString defaultPaths;
    wxArrayString gccCompilers;
    gccCompilers.Add(COMPILER_FAMILY_MINGW);
    gccCompilers.Add(COMPILER_FAMILY_CLANG);
    gccCompilers.Add(COMPILER_FAMILY_GCC);

    // Only add the cygwin
    if(::clIsCygwinEnvironment()) { gccCompilers.Add(COMPILER_FAMILY_CYGWIN); }

    if(gccCompilers.Index(GetCompilerFamily()) != wxNOT_FOUND) { defaultPaths = POSIXGetIncludePaths(); }
    return defaultPaths;
}

wxString Compiler::GetGCCVersion() const
{
    // Get the compiler version
    static wxRegEx reVersion("([0-9]+\\.[0-9]+\\.[0-9]+)");
    wxString command;
    command << GetTool("CXX") << " --version";
    wxArrayString out;
    ProcUtils::SafeExecuteCommand(command, out);
    if(out.IsEmpty()) { return ""; }

    if(reVersion.Matches(out.Item(0))) { return reVersion.GetMatch(out.Item(0)); }
    return "";
}

wxString Compiler::GetIncludePath(const wxString& pathSuffix) const
{
    wxString fullpath;
    fullpath << GetInstallationPath() << "/" << pathSuffix;
    wxFileName fn(fullpath, "");
    return fn.GetPath();
}

wxArrayString Compiler::POSIXGetIncludePaths() const
{
    wxString command;
    clDEBUG() << "Loading compiler built-in search paths...";
#ifdef __WXMSW__
    if(::clIsCygwinEnvironment()) {
        command << GetTool("CXX") << " -v -x c++ /dev/null -fsyntax-only";
    } else {
        command << GetTool("CXX") << " -v -x c++ nul -fsyntax-only";
    }
#else
    command << GetTool("CXX") << " -v -x c++ /dev/null -fsyntax-only";
#endif

    clDEBUG() << "Running command:" << command;
    wxString outputStr;
    IProcess::Ptr_t proc(::CreateSyncProcess(command));
    if(proc) proc->WaitForTerminate(outputStr);

    clDEBUG() << "Output is:" << outputStr;

    wxArrayString arr;
    wxArrayString outputArr = ::wxStringTokenize(outputStr, wxT("\n\r"), wxTOKEN_STRTOK);

    // Analyze the output
    bool collect(false);
    for(size_t i = 0; i < outputArr.GetCount(); i++) {
        if(outputArr[i].Contains(wxT("#include <...> search starts here:"))) {
            collect = true;
            continue;
        }

        if(outputArr[i].Contains(wxT("End of search list."))) { break; }

        if(collect) {

            wxString file = outputArr.Item(i).Trim().Trim(false);

            // on Mac, (framework directory) appears also,
            // but it is harmless to use it under all OSs
            file.Replace(wxT("(framework directory)"), wxT(""));
            file.Trim().Trim(false);

            // Fix cygwin paths to use Windows native paths
#ifdef __WXMSW__
            if(GetCompilerFamily() == COMPILER_FAMILY_CYGWIN) {
                const wxString& cygdriveRoot = GetInstallationPath();

                // For reasons beyond me, /usr/lib is mapped to /lib
                if(file.StartsWith("/usr/lib")) { file.Replace("/usr/lib", "/lib"); }
                file.Prepend(cygdriveRoot + "/");
            }
#endif

            wxFileName includePath(file, "");
            includePath.Normalize();

            arr.Add(includePath.GetPath());
        }
    }
    return arr;
}

const wxArrayString& Compiler::GetBuiltinMacros()
{
    if(!m_compilerBuiltinDefinitions.IsEmpty()) { return m_compilerBuiltinDefinitions; }

    wxArrayString definitions;
    // Command example: "echo | clang -dM -E - > /tmp/pp"
    if(GetCompilerFamily() == COMPILER_FAMILY_CLANG || GetCompilerFamily() == COMPILER_FAMILY_GCC ||
       GetCompilerFamily() == COMPILER_FAMILY_CYGWIN || GetCompilerFamily() == COMPILER_FAMILY_MINGW) {
        wxString command;
        wxString tool = GetTool("CXX");
        tool.Trim().Trim(false);
        command << "echo | \"" << tool << "\" -dM -E - > ";
        wxString tmpFile = wxFileName::CreateTempFileName("def-macros");
        ::WrapWithQuotes(tmpFile);
        command << tmpFile;
        ::WrapInShell(command);
        // CL_SYSTEM(command);

        ProcUtils::SafeExecuteCommand(command);
        wxFileName cmpMacrosFile(tmpFile);
        if(cmpMacrosFile.Exists()) {
            clDEBUG1() << "Compiler builtin macros are written into:" << cmpMacrosFile.GetFullPath();
            // we got our macro files
            {
                CxxPreProcessor pp;
                pp.Parse(cmpMacrosFile, kLexerOpt_CollectMacroValueNumbers);
                definitions = pp.GetDefinitions();
            }

            //            for(size_t i = 0; i < definitions.GetCount(); ++i) {
            //                clDEBUG1() << "BUILTIN:" << definitions.Item(i);
            //            }

            {
                // Delete the file
                clRemoveFile(cmpMacrosFile.GetFullPath());
            }
        }
    }
    m_compilerBuiltinDefinitions.swap(definitions);
    return m_compilerBuiltinDefinitions;
}

wxString Compiler::GetLinkLine(const wxString& type, bool inputFromFile) const
{
    wxString customType = type;
    const auto& iter = m_linkerLines.find(customType);
    if(iter == m_linkerLines.end()) { return ""; }
    return inputFromFile ? iter->second.lineFromFile : iter->second.line;
}

void Compiler::SetLinkLine(const wxString& type, const wxString& line, bool inputFromFile)
{
    auto where = m_linkerLines.find(type);
    if(where == m_linkerLines.end()) {
        m_linkerLines.insert({ type, { "", "" } });
        where = m_linkerLines.find(type);
    }
    if(inputFromFile) {
        where->second.lineFromFile = line;
    } else {
        where->second.line = line;
    }
}
