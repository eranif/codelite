#ifndef CLANGLOCALPATHS_H
#define CLANGLOCALPATHS_H

#if HAS_LIBCLANG

#include "precompiled_header.h"
#include <wx/filename.h>
#include <wx/xml/xml.h>
#include "serialized_object.h"

class ClangCodeCompletionOptions : public SerializedObject
{
public:
	typedef std::set<wxString> Set_t;

protected:
	Set_t    m_searchPaths;
	Set_t    m_macros;
	Set_t    m_frameworks;
	wxString m_name;

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
	void UpdateFrameworks(const Set_t& frameworks);
	void UpdateMacros(const Set_t& macros) ;
	void UpdateSearchPaths(const Set_t& searchPaths) ;
	
	Set_t& GetMacros() {
		return m_macros;
	}
	Set_t& GetSearchPaths() {
		return m_searchPaths;
	}
	
	Set_t& GetFrameworks() {
		return m_frameworks;
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
#endif // HAS_LIBCLANG
#endif // CLANGLOCALPATHS_H
