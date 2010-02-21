//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_options_data.cpp
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
#include "precompiled_header.h"
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include "tags_options_data.h"

extern bool IsValidCppIndetifier(const wxString &id);
extern bool IsCppKeyword(const wxString &word);

//---------------------------------------------------------

TagsOptionsData::TagsOptionsData()
		: SerializedObject()
		, m_ccFlags       (CC_DISP_FUNC_CALLTIP | CC_LOAD_EXT_DB | CC_CPP_KEYWORD_ASISST | CC_COLOUR_VARS | CC_ACCURATE_SCOPE_RESOLVING)
		, m_ccColourFlags (CC_COLOUR_DEFAULT)
		, m_fileSpec(wxT  ("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc"))
		, m_minWordLen    (3)
		, m_parserEnabled (true)
		, m_maxItemToColour(1000)
{
	SetVersion(wxT("2.3"));
	// Initialize defaults
	m_languages.Add(wxT("C++"));
	m_tokens =
wxT("EXPORT\n")
wxT("WXDLLIMPEXP_CORE\n")
wxT("WXDLLIMPEXP_BASE\n")
wxT("WXDLLIMPEXP_XML\n")
wxT("WXDLLIMPEXP_XRC\n")
wxT("WXDLLIMPEXP_ADV\n")
wxT("WXDLLIMPEXP_AUI\n")
wxT("WXDLLIMPEXP_CL\n")
wxT("WXDLLIMPEXP_LE_SDK\n")
wxT("WXDLLIMPEXP_SQLITE3\n")
wxT("WXMAKINGDLL\n")
wxT("WXUSINGDLL\n")
wxT("_CRTIMP\n")
wxT("__CRT_INLINE\n")
wxT("__cdecl\n")
wxT("__stdcall\n")
wxT("WXDLLEXPORT\n")
wxT("WXDLLIMPORT\n")
wxT("wxT\n")
#if defined (__WXGTK__)
	wxT("wxTopLevelWindowNative=wxTopLevelWindowGTK\n")
	wxT("wxWindow=wxWindowGTK\n")
#elif defined(__WXMSW__)
	wxT("wxTopLevelWindowNative=wxTopLevelWindowMSW\n")
	wxT("wxWindow=wxWindowMSW\n")
#else
	wxT("wxTopLevelWindowNative=wxTopLevelWindowMac\n")
	wxT("wxWindow=wxWindowMac\n")
#endif
wxT("wxWindowNative=wxWindowBase\n")
wxT("wxStatusBar=wxStatusBarBase\n")
wxT("BEGIN_DECLARE_EVENT_TYPES()=enum {\n")
wxT("END_DECLARE_EVENT_TYPES()=};\n")
wxT("DECLARE_EVENT_TYPE\n")
wxT("DECLARE_EXPORTED_EVENT_TYPE\n")
wxT("WXUNUSED+\n")
wxT("wxDEPRECATED\n")
wxT("_T\n")
wxT("ATTRIBUTE_PRINTF_1\n")
wxT("ATTRIBUTE_PRINTF_2\n")
wxT("WXDLLIMPEXP_FWD_BASE\n")
wxT("WXDLLIMPEXP_FWD_CORE\n")
wxT("DLLIMPORT\n")
wxT("DECLARE_INSTANCE_TYPE\n")
wxT("emit\n")
wxT("Q_OBJECT\n")
wxT("Q_PACKED\n")
wxT("Q_GADGET\n")
wxT("QT_BEGIN_HEADER\n")
wxT("QT_END_HEADER\n")
wxT("Q_REQUIRED_RESULT\n")
wxT("Q_INLINE_TEMPLATE\n")
wxT("Q_OUTOFLINE_TEMPLATE\n")
wxT("_GLIBCXX_BEGIN_NAMESPACE(std)=namespace std{\n")
wxT("_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)=namespace __gnu_cxx{\n")
wxT("_GLIBCXX_END_NAMESPACE=}\n")
wxT("_GLIBCXX_BEGIN_NESTED_NAMESPACE(std, _GLIBCXX_STD)=namespace std{\n")
wxT("_GLIBCXX_BEGIN_NESTED_NAMESPACE(std, _GLIBCXX_STD_D)=namespace std{\n")
wxT("_GLIBCXX_END_NESTED_NAMESPACE=}\n")
wxT("_GLIBCXX_STD=std\n")
wxT("__const=const\n")
wxT("__restrict\n")
wxT("__THROW\n")
wxT("__wur\n")
wxT("_STD_BEGIN=namespace std{\n")
wxT("_STD_END=}\n")
wxT("__CLRCALL_OR_CDECL\n")
wxT("_CRTIMP2_PURE");

	m_types =
wxT("std::vector::reference=_Tp\n")
wxT("std::vector::const_reference=_Tp\n")
wxT("std::vector::iterator=_Tp\n")
wxT("std::vector::const_iterator=_Tp\n")
wxT("std::queue::reference=_Tp\n")
wxT("std::queue::const_reference=_Tp\n")
wxT("std::set::const_iterator=_Key\n")
wxT("std::set::iterator=_Key\n")
wxT("std::deque::reference=_Tp\n")
wxT("std::deque::const_reference=_Tp\n")
wxT("std::map::iterator=pair<typename _T1, typename _T2>\n")
wxT("std::map::const_iterator=pair<typename _T1, typename _T2>\n")
wxT("std::multimap::iterator=pair<typename _T1, typename _T2>\n")
wxT("std::multimap::const_iterator=pair<typename _T1, typename _T2>");
}

TagsOptionsData::~TagsOptionsData()
{
}

void TagsOptionsData::Serialize(Archive &arch)
{
	// since of build 3749, we *always* set CC_ACCURATE_SCOPE_RESOLVING to true
	m_ccFlags |= CC_ACCURATE_SCOPE_RESOLVING;

	arch.Write     (wxT("m_ccFlags"),           m_ccFlags);
	arch.Write     (wxT("m_ccColourFlags"),     m_ccColourFlags);
	arch.WriteCData(wxT("m_tokens"),            m_tokens);
	arch.WriteCData(wxT("m_types"),             m_types);
	arch.Write     (wxT("m_fileSpec"),          m_fileSpec);
	arch.Write     (wxT("m_languages"),         m_languages);
	arch.Write     (wxT("m_minWordLen"),        m_minWordLen);
	arch.Write     (wxT("m_parserSearchPaths"), m_parserSearchPaths);
	arch.Write     (wxT("m_parserEnabled"),     m_parserEnabled);
	arch.Write     (wxT("m_parserExcludePaths"),m_parserExcludePaths);
	arch.Write     (wxT("m_maxItemToColour"),   m_maxItemToColour);
}

void TagsOptionsData::DeSerialize(Archive &arch)
{
	arch.Read     (wxT("m_ccFlags"),           m_ccFlags);
	arch.Read     (wxT("m_ccColourFlags"),     m_ccColourFlags);
	arch.ReadCData(wxT("m_tokens"),            m_tokens);
	arch.ReadCData(wxT("m_types"),             m_types);
	arch.Read     (wxT("m_fileSpec"),          m_fileSpec);
	arch.Read     (wxT("m_languages"),         m_languages);
	arch.Read     (wxT("m_minWordLen"),        m_minWordLen);
	arch.Read     (wxT("m_parserSearchPaths"), m_parserSearchPaths);
	arch.Read     (wxT("m_parserEnabled"),     m_parserEnabled);
	arch.Read     (wxT("m_parserExcludePaths"),m_parserExcludePaths);
	arch.Read     (wxT("m_maxItemToColour"),   m_maxItemToColour);

	// since of build 3749, we *always* set CC_ACCURATE_SCOPE_RESOLVING to true
	m_ccFlags |= CC_ACCURATE_SCOPE_RESOLVING;
}

wxString TagsOptionsData::ToString() const
{
	wxString options(wxEmptyString);

	wxString file_name, file_content;
	wxGetEnv(wxT("CTAGS_REPLACEMENTS"), &file_name);

	std::map<wxString, wxString> tokensMap = GetTokensWxMap();
	std::map<wxString, wxString>::iterator iter = tokensMap.begin();

	if(tokensMap.empty() == false) {
		options = wxT(" -I");
		for(; iter != tokensMap.end(); iter++) {
			if(iter->second.IsEmpty() == false) {
				// Key = Value pair. Place this one in the output file
				file_content << iter->first << wxT("=") << iter->second << wxT("\n");
			} else {
				options << iter->first;
				options << wxT(",");
			}
		}
		options.RemoveLast();
		options += wxT(" ");
	}

	// write the file content
	if (file_name.IsEmpty() == false) {
		wxFFile fp(file_name, wxT("w+b"));
		if (fp.IsOpened()) {
			fp.Write(file_content);
			fp.Close();
		}
	}

	if (GetLanguages().IsEmpty() == false) {
		options += wxT(" --language-force=");
		options += GetLanguages().Item(0);
		options += wxT(" ");
	}
	return options;
}

void TagsOptionsData::SetLanguageSelection(const wxString &lang)
{
	int where = m_languages.Index(lang);
	if (where != wxNOT_FOUND) {
		m_languages.RemoveAt(where);
	}
	m_languages.Insert(lang, 0);
}

std::map<std::string,std::string> TagsOptionsData::GetTokensMap() const
{
	std::map<std::string,std::string> tokens;
	wxArrayString tokensArr = wxStringTokenize(m_tokens, wxT("\r\n"), wxTOKEN_STRTOK);
	for (size_t i=0; i<tokensArr.GetCount(); i++) {
		//const wxCharBuffer bufKey = _C(
		wxString item = tokensArr.Item(i).Trim().Trim(false);
		wxString k = item.BeforeFirst(wxT('='));
		wxString v = item.AfterFirst(wxT('='));

		const wxCharBuffer bufKey = _C(k);
		std::string key = bufKey.data();
		std::string value;
		if (!v.empty()) {
			const wxCharBuffer bufValue = _C(v);
			value = bufValue.data();
		}
		tokens[key] = value;
	}
	return tokens;
}

std::map<wxString, wxString> TagsOptionsData::GetTokensWxMap() const
{
	std::map<wxString, wxString> tokens;
	wxArrayString tokensArr = wxStringTokenize(m_tokens, wxT("\r\n"), wxTOKEN_STRTOK);
	for (size_t i=0; i<tokensArr.GetCount(); i++) {
		wxString item = tokensArr.Item(i).Trim().Trim(false);
		wxString k = item.BeforeFirst(wxT('='));
		wxString v = item.AfterFirst(wxT('='));
		tokens[k] = v;
	}
	return tokens;
}

std::map<wxString,wxString> TagsOptionsData::GetTypesMap() const
{
	std::map<wxString, wxString> tokens;
	wxArrayString typesArr = wxStringTokenize(m_types, wxT("\r\n"), wxTOKEN_STRTOK);
	for (size_t i=0; i<typesArr.GetCount(); i++) {
		wxString item = typesArr.Item(i).Trim().Trim(false);
		wxString k = item.BeforeFirst(wxT('='));
		wxString v = item.AfterFirst(wxT('='));
		tokens[k] = v;
	}
	return tokens;
}

std::map<std::string, std::string> TagsOptionsData::GetTokensReversedMap() const
{
	std::map<std::string, std::string> tokens;
	wxArrayString typesArr = wxStringTokenize(m_tokens, wxT("\r\n"), wxTOKEN_STRTOK);
	for (size_t i=0; i<typesArr.GetCount(); i++) {
		wxString item = typesArr.Item(i).Trim().Trim(false);
		wxString k = item.AfterFirst(wxT('='));
		wxString v = item.BeforeFirst(wxT('='));

		if(IsValidCppIndetifier(k) && !IsCppKeyword(k)) {
			tokens[k.mb_str(wxConvUTF8).data()] = v.mb_str(wxConvUTF8).data();
		}
	}
	return tokens;
}

std::map<wxString,wxString> TagsOptionsData::GetTokensReversedWxMap() const
{
	std::map<wxString, wxString> tokens;
	wxArrayString typesArr = wxStringTokenize(m_tokens, wxT("\r\n"), wxTOKEN_STRTOK);
	for (size_t i=0; i<typesArr.GetCount(); i++) {
		wxString item = typesArr.Item(i).Trim().Trim(false);
		wxString k = item.AfterFirst(wxT('='));
		wxString v = item.BeforeFirst(wxT('='));

		if(IsValidCppIndetifier(k) && !IsCppKeyword(k)) {
			tokens[k] = v;
		}
	}
	return tokens;
}
