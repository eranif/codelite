#if HAS_LIBCLANG

#include "clang_local_paths.h"
#include "globals.h"
#include <wx/ffile.h>
#include <wx/log.h>
#include <wx/filename.h>

void ClangCodeCompletionOptions::DeSerialize(Archive& arch)
{
	arch.Read(wxT("m_macros"),      m_macros);
	arch.Read(wxT("m_searchPaths"), m_searchPaths);
}

void ClangCodeCompletionOptions::Serialize(Archive& arch)
{
	arch.Write(wxT("m_macros"),      m_macros);
	arch.Write(wxT("m_searchPaths"), m_searchPaths);
}

void ClangCodeCompletionOptions::UpdateSearchPaths(const Set_t& searchPaths)
{
	m_searchPaths.insert(searchPaths.begin(), searchPaths.end());
}

void ClangCodeCompletionOptions::UpdateMacros(const Set_t& macros)
{
	m_macros.insert(macros.begin(), macros.end());
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

ClangLocalPaths::ClangLocalPaths(const wxFileName &projectFile)
{
	m_isOk = false;
	wxLogNull noLog;
	m_filename = projectFile;
	m_filename.SetExt(wxT("clang"));

	if(!m_filename.FileExists()) {
		wxFFile fp(m_filename.GetFullPath(), wxT("w+b"));
		if(fp.IsOpened()) {
			fp.Write(wxT("<ClangCodeCompletion/>"));
			fp.Close();
		}
	}

	m_isOk = m_doc.Load(m_filename.GetFullPath());
	if(!m_isOk) {
		wxFFile fp(m_filename.GetFullPath(), wxT("w+b"));
		if(fp.IsOpened()) {
			fp.Write(wxT("<ClangCodeCompletion/>"));
			fp.Close();
		}

		m_isOk = m_doc.Load(m_filename.GetFullPath());
	}

	if(m_isOk) {
		wxXmlNode *root = m_doc.GetRoot();
		wxXmlNode *child = root->GetChildren();
		while ( child ) {

			if(child->GetName() == wxT("Configuration")) {
				wxString confName;
				confName = child->GetPropVal(wxT("Name"), wxEmptyString);
				ClangCodeCompletionOptions c;

				Archive ar;
				ar.SetXmlNode(child);
				c.DeSerialize(ar);
				c.SetName(confName);
				m_confOptions[confName] = c;
			}
			child = child->GetNext();
		}
	}
}

ClangLocalPaths::~ClangLocalPaths()
{
}

void ClangLocalPaths::Save()
{
	if(!m_isOk) return;

	// Delete all XML
	wxXmlNode *root = m_doc.GetRoot();
	wxXmlNode *child = root->GetChildren();
	while (child) {
		wxXmlNode *n = child->GetNext();

		root->RemoveChild(child);
		delete child;

		child = n;
	}

	Map_t::iterator iter = m_confOptions.begin();
	for(; iter != m_confOptions.end(); iter++) {
		wxXmlNode *node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Configuration"));
		node->AddProperty(wxT("Name"), iter->first);
		Archive arch;
		arch.SetXmlNode(node);
		iter->second.Serialize(arch);
		root->AddChild(node);
	}

	m_doc.Save(m_filename.GetFullPath());
}

ClangCodeCompletionOptions& ClangLocalPaths::Options(const wxString& config)
{
	Map_t::iterator iter = m_confOptions.find(config);
	if(iter == m_confOptions.end()) {
		ClangCodeCompletionOptions c;
		c.SetName(config);
		m_confOptions[config] = c;
	}
	iter = m_confOptions.find(config);
	return iter->second;
}

#endif // HAS_LIBCLANG

