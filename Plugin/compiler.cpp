#include "compiler.h"
#include "xmlutils.h"

Compiler::Compiler(wxXmlNode *node)
{
	if(node){
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("Switch")){
				m_switches[XmlUtils::ReadString(child, wxT("Name"))] = XmlUtils::ReadString(child, wxT("Value"));
			}

			else if(child->GetName() == wxT("Tool")){
				m_tools[XmlUtils::ReadString(child, wxT("Name"))] = XmlUtils::ReadString(child, wxT("Value"));
			}

			else if(child->GetName() == wxT("Option")){
				if(XmlUtils::ReadString(child, wxT("Name")) == wxT("ObjectSuffix")){
					m_objectSuffix = XmlUtils::ReadString(child, wxT("Value"));
				}
			}

			else if(child->GetName() == wxT("Pattern")){
				if(XmlUtils::ReadString(child, wxT("Name")) == wxT("Error")){
					//found the error description
					m_errorFileNameIndex = XmlUtils::ReadString(child, wxT("FileNameIndex"));
					m_errorLineNubmerIndex = XmlUtils::ReadString(child, wxT("LineNumberIndex"));
					m_errorPattern = child->GetNodeContent();
				}else if(XmlUtils::ReadString(child, wxT("Name")) == wxT("Warning")){
					//found the warning description
					m_warningFileNameIndex = XmlUtils::ReadString(child, wxT("FileNameIndex"));
					m_warningLineNubmerIndex = XmlUtils::ReadString(child, wxT("LineNumberIndex"));
					m_warningPattern = child->GetNodeContent();
				}
			}
			
			else if(child->GetName() == wxT("GlobalIncludePath")){
				m_globalIncludePath = child->GetNodeContent();
			}
			
			else if(child->GetName() == wxT("GlobalLibPath")){
				m_globalLibPath = child->GetNodeContent();
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
		m_objectSuffix = wxT(".o");
		m_errorPattern = wxT("(^[a-zA-Z\\.0-9_/:\\-]+ *)(:)([0-9]+)(:)([ eor]*)");
		m_errorFileNameIndex = wxT("1");
		m_errorLineNubmerIndex = wxT("3");
		m_warningPattern = wxT("(^[a-zA-Z\\.0-9_/:\\-]+ *)(:)([0-9]+ *)(:)([0-9:]*)( warning:)");
		m_warningFileNameIndex = wxT("1");
		m_warningLineNubmerIndex = wxT("3");
		m_tools[wxT("LinkerName")] = wxT("g++");
		m_tools[wxT("SharedObjectLinkerName")] = wxT("g++ -shared -fPIC");
		m_tools[wxT("CompilerName")] = wxT("g++");
		m_tools[wxT("ArchiveTool")] = wxT("ar rcu");
		m_tools[wxT("ResourceCompiler")] = wxT("windres");
		m_globalIncludePath = wxEmptyString;
		m_globalLibPath = wxEmptyString;
	}
}

Compiler::~Compiler()
{
}

wxXmlNode *Compiler::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compiler"));
	node->AddProperty(wxT("Name"), m_name);
	
	std::map<wxString, wxString>::const_iterator iter = m_switches.begin();
	for(; iter != m_switches.end(); iter++){
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Switch"));
		child->AddProperty(wxT("Name"), iter->first);
		child->AddProperty(wxT("Value"), iter->second);
		node->AddChild(child);
	}

	iter = m_tools.begin();
	for(; iter != m_tools.end(); iter++){
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Tool"));
		child->AddProperty(wxT("Name"), iter->first);
		child->AddProperty(wxT("Value"), iter->second);
		node->AddChild(child);
	}

	wxXmlNode *options = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
	options->AddProperty(wxT("Name"), wxT("ObjectSuffix"));
	options->AddProperty(wxT("Value"), m_objectSuffix);
	node->AddChild(options);

	//add patterns
	wxXmlNode *error = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Pattern"));
	error->AddProperty(wxT("Name"), wxT("Error"));
	error->AddProperty(wxT("FileNameIndex"), m_errorFileNameIndex);
	error->AddProperty(wxT("LineNumberIndex"), m_errorLineNubmerIndex);
	XmlUtils::SetNodeContent(error, m_errorPattern);
	node->AddChild(error);

	wxXmlNode *warning = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Pattern"));
	warning->AddProperty(wxT("Name"), wxT("Warning"));
	warning->AddProperty(wxT("FileNameIndex"), m_warningFileNameIndex);
	warning->AddProperty(wxT("LineNumberIndex"), m_warningLineNubmerIndex);
	XmlUtils::SetNodeContent(warning, m_warningPattern);
	node->AddChild(warning);
	
	wxXmlNode *globalIncludePath = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("GlobalIncludePath"));
	XmlUtils::SetNodeContent(globalIncludePath, m_globalIncludePath);
	node->AddChild(globalIncludePath);
	
	wxXmlNode *globalLibPath = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("GlobalLibPath"));
	XmlUtils::SetNodeContent(globalLibPath, m_globalLibPath);
	node->AddChild(globalLibPath);
	return node;
}

wxString Compiler::GetSwitch(const wxString &name) const
{
	std::map<wxString, wxString>::const_iterator iter = m_switches.find(name);
	if(iter == m_switches.end()){
		return wxEmptyString;
	}
	return iter->second;
}

wxString Compiler::GetTool(const wxString &name) const
{
	std::map<wxString, wxString>::const_iterator iter = m_tools.find(name);
	if(iter == m_tools.end()){
		return wxEmptyString;
	}
	return iter->second;
}
