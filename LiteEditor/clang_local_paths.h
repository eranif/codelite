#ifndef CLANGLOCALPATHS_H
#define CLANGLOCALPATHS_H

#include "precompiled_header.h"
#include <wx/filename.h>
#include <wx/xml/xml.h>
#include "serialized_object.h"

class ClangCodeCompletionOptions : public SerializedObject
{
	std::set<wxString> m_searchPaths;
	std::set<wxString> m_macros;
	wxString           m_name;

public:
	ClangCodeCompletionOptions() {}
	virtual ~ClangCodeCompletionOptions() {}

	void DeSerialize(Archive& arch);
	void Serialize(Archive& arch);

	void SetName(const wxString& name) {
		this->m_name = name;
	}
	const wxString& GetName() const {
		return m_name;
	}
	void SetMacros(const std::set<wxString>& macros) {
		this->m_macros = macros;
	}
	void SetSearchPaths(const std::set<wxString>& searchPaths) {
		this->m_searchPaths = searchPaths;
	}
	std::set<wxString>& GetMacros() {
		return m_macros;
	}
	std::set<wxString>& GetSearchPaths() {
		return m_searchPaths;
	}
};

class ClangLocalPaths
{
public:
	typedef std::map<wxString, ClangCodeCompletionOptions> Map_t;

protected:
	wxXmlDocument m_doc;
	wxFileName    m_filename;
	bool          m_isOk;
	Map_t         m_confOptions;

public:
	ClangLocalPaths(const wxFileName &projectFile);
	virtual ~ClangLocalPaths();

	ClangCodeCompletionOptions& Options(const wxString &config);
	void Save();
};

#endif // CLANGLOCALPATHS_H
