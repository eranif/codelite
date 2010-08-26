#include "build_config_common.h"
#include "globals.h"
#include "xmlutils.h"
#include <wx/tokenzr.h>
#include "macros.h"
#include "wx_xml_compatibility.h"

BuildConfigCommon::BuildConfigCommon(wxXmlNode* node, wxString confType)
: m_confType(confType)
{
	if (node) {
		// read the compile options
		wxXmlNode *compile = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
		if (compile) {
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

		// read the linker options
		wxXmlNode *linker = XmlUtils::FindFirstByTagName(node, wxT("Linker"));
		if (linker) {
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

		// read the resource compile options
		wxXmlNode *resCmp = XmlUtils::FindFirstByTagName(node, wxT("ResourceCompiler"));
		if (resCmp) {
			m_resCompileOptions = XmlUtils::ReadString(resCmp, wxT("Options"));
			wxXmlNode *child = resCmp->GetChildren();
			while (child) {
				if (child->GetName() == wxT("IncludePath")) {
					m_resCmpIncludePath << XmlUtils::ReadString(child, wxT("Value")) << wxT(";");
				}
				child = child->GetNext();
			}
		}
	}
	else {
		m_includePath.Add(wxT("."));
		m_libPath.Add(wxT("."));
	}
}

BuildConfigCommon::~BuildConfigCommon()
{

}

wxXmlNode *BuildConfigCommon::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, m_confType);

	//create the compile node
	wxXmlNode *compile = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compiler"));
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

	//add the resource compiler node
	wxXmlNode *resCmp = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ResourceCompiler"));
	resCmp->AddProperty(wxT("Options"), m_resCompileOptions);
	node->AddChild(resCmp);

	wxStringTokenizer tok(m_resCmpIncludePath, wxT(";"));
	while (tok.HasMoreTokens()) {
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("IncludePath"));
		option->AddProperty(wxT("Value"),tok.NextToken());
		resCmp->AddChild(option);
	}
	return node;
}

void BuildConfigCommon::SetPreprocessor(const wxString &pre)
{
	FillFromSmiColonString(m_preprocessor, pre);
}

void BuildConfigCommon::SetIncludePath(const wxString &path)
{
	FillFromSmiColonString(m_includePath, path);
}

void BuildConfigCommon::SetLibraries(const wxString &libs)
{
	FillFromSmiColonString(m_libs, libs);
}

void BuildConfigCommon::SetLibPath(const wxString &paths)
{
	FillFromSmiColonString(m_libPath, paths);
}

wxString BuildConfigCommon::GetLibPath() const
{
	return ArrayToSmiColonString(m_libPath);
}

wxString BuildConfigCommon::GetLibraries() const
{
	return ArrayToSmiColonString(m_libs);
}

wxString BuildConfigCommon::GetIncludePath() const
{
	return ArrayToSmiColonString(m_includePath);
}

wxString BuildConfigCommon::GetPreprocessor() const
{
	wxString asString;
	for (size_t i=0; i<m_preprocessor.GetCount(); i++) {
		wxString tmp = m_preprocessor.Item(i);
		tmp.Trim().Trim(false);
		if(tmp.IsEmpty())
			continue;

		asString << tmp << wxT(";");

	}
	if(asString.IsEmpty() == false)
		asString.RemoveLast();

	return asString;
}

