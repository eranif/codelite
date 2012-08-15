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
#include "istorage.h"
#include <wx/tokenzr.h>
#include <wx/ffile.h>
#include "tags_options_data.h"
#include <set>

wxString TagsOptionsData::CLANG_CACHE_LAZY         = wxT("Lazy");
wxString TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD = wxT("On File Load");

static bool _IsValidCppIndetifier(const wxString &id)
{
	if (id.IsEmpty()) {
		return false;
	}
	//first char can be only _A-Za-z
	wxString first( id.Mid(0, 1) );
	if (first.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")) != wxString::npos ) {
		return false;
	}
	//make sure that rest of the id contains only a-zA-Z0-9_
	if (id.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")) != wxString::npos) {
		return false;
	}
	return true;
}

static bool _IsCppKeyword(const wxString &word)
{
	static std::set<wxString> words;

	if(words.empty()) {
		words.insert(wxT("auto"));
		words.insert(wxT("break"));
		words.insert(wxT("case"));
		words.insert(wxT("char"));
		words.insert(wxT("const"));
		words.insert(wxT("continue"));
		words.insert(wxT("default"));
		words.insert(wxT("define"));
		words.insert(wxT("defined"));
		words.insert(wxT("do"));
		words.insert(wxT("double"));
		words.insert(wxT("elif"));
		words.insert(wxT("else"));
		words.insert(wxT("endif"));
		words.insert(wxT("enum"));
		words.insert(wxT("error"));
		words.insert(wxT("extern"));
		words.insert(wxT("float"));
		words.insert(wxT("for"));
		words.insert(wxT("goto"));
		words.insert(wxT("if"));
		words.insert(wxT("ifdef"));
		words.insert(wxT("ifndef"));
		words.insert(wxT("include"));
		words.insert(wxT("int"));
		words.insert(wxT("line"));
		words.insert(wxT("long"));
		words.insert(wxT("bool"));
		words.insert(wxT("pragma"));
		words.insert(wxT("register"));
		words.insert(wxT("return"));
		words.insert(wxT("short"));
		words.insert(wxT("signed"));
		words.insert(wxT("sizeof"));
		words.insert(wxT("static"));
		words.insert(wxT("struct"));
		words.insert(wxT("switch"));
		words.insert(wxT("typedef"));
		words.insert(wxT("undef"));
		words.insert(wxT("union"));
		words.insert(wxT("unsigned"));
		words.insert(wxT("void"));
		words.insert(wxT("volatile"));
		words.insert(wxT("while"));
		words.insert(wxT("class"));
		words.insert(wxT("namespace"));
		words.insert(wxT("delete"));
		words.insert(wxT("friend"));
		words.insert(wxT("inline"));
		words.insert(wxT("new"));
		words.insert(wxT("operator"));
		words.insert(wxT("overload"));
		words.insert(wxT("protected"));
		words.insert(wxT("private"));
		words.insert(wxT("public"));
		words.insert(wxT("this"));
		words.insert(wxT("virtual"));
		words.insert(wxT("template"));
		words.insert(wxT("typename"));
		words.insert(wxT("dynamic_cast"));
		words.insert(wxT("static_cast"));
		words.insert(wxT("const_cast"));
		words.insert(wxT("reinterpret_cast"));
		words.insert(wxT("using"));
		words.insert(wxT("throw"));
		words.insert(wxT("catch"));
	}

	return words.find(word) != words.end();
}

//---------------------------------------------------------

TagsOptionsData::TagsOptionsData()
		: SerializedObject()
		, m_ccFlags       (CC_DISP_FUNC_CALLTIP | CC_LOAD_EXT_DB | CC_CPP_KEYWORD_ASISST | CC_COLOUR_VARS | CC_ACCURATE_SCOPE_RESOLVING | CC_PARSE_EXT_LESS_FILES)
		, m_ccColourFlags (CC_COLOUR_DEFAULT)
		, m_fileSpec(wxT  ("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc"))
		, m_minWordLen    (3)
		, m_parserEnabled (true)
		, m_maxItemToColour(1000)
#ifdef __WXMSW__
		, m_macrosFiles   (wxT("_mingw.h bits/c++config.h"))
#elif defined(__WXMAC__)
		, m_macrosFiles   (wxT("sys/cdefs.h bits/c++config.h AvailabilityMacros.h"))
#else
		, m_macrosFiles   (wxT("sys/cdefs.h bits/c++config.h"))
#endif
		, m_clangOptions(0)
		, m_clangBinary(wxT(""))
		, m_clangCachePolicy(TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD)
		, m_ccNumberOfDisplayItems(MAX_SEARCH_LIMIT)
{
	SetVersion(wxT("3.0.10"));
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
wxT("WXDLLIMPEXP_FWD_ADV\n")
wxT("WXDLLIMPEXP_CL\n")
wxT("WXDLLIMPEXP_LE_SDK\n")
wxT("WXDLLIMPEXP_SQLITE3\n")
wxT("WXDLLIMPEXP_SCI\n")
wxT("WXDLLIMPEXP_FWD_AUI\n")
wxT("WXMAKINGDLL\n")
wxT("WXUSINGDLL\n")
wxT("_CRTIMP\n")
wxT("__CRT_INLINE\n")
wxT("__cdecl\n")
wxT("__stdcall\n")
wxT("WXDLLEXPORT\n")
wxT("WXDLLIMPORT\n")
wxT("__MINGW_ATTRIB_PURE\n")
wxT("__MINGW_ATTRIB_MALLOC\n")
wxT("__GOMP_NOTHROW\n")
wxT("wxT\n")
wxT("SCI_SCOPE(%0)=%0\n")
wxT("WINBASEAPI\n")
wxT("WINAPI\n")
wxT("__nonnull\n")
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
wxT("WXUNUSED(%0)=%0\n")
wxT("wxDEPRECATED(%0)=%0\n")
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
wxT("_GLIBCXX_BEGIN_NAMESPACE(%0)=namespace %0{\n")
wxT("_GLIBCXX_END_NAMESPACE=}\n")
wxT("_GLIBCXX_BEGIN_NESTED_NAMESPACE(%0, %1)=namespace %0{\n")
wxT("wxDECLARE_EXPORTED_EVENT(%0,%1,%2)=int %1;\n")
wxT("DECLARE_EVENT_TYPE(%0,%1)=int %0;\n")
wxT("_GLIBCXX_END_NESTED_NAMESPACE=}\n")
wxT("_GLIBCXX_VISIBILITY(%0)\n")
wxT("_GLIBCXX_BEGIN_NAMESPACE_TR1=namespace tr1{\n")
wxT("_GLIBCXX_END_NAMESPACE_TR1=}\n")
wxT("_GLIBCXX_STD=std\n")
wxT("_GLIBCXX_BEGIN_NAMESPACE_CONTAINER\n")
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
wxT("std::map::iterator=std::pair<_Key, _Tp>\n")
wxT("std::map::const_iterator=std::pair<_Key,_Tp>\n")
wxT("std::multimap::iterator=std::pair<_Key,_Tp>\n")
wxT("std::multimap::const_iterator=std::pair<_Key,_Tp>\n")
wxT("wxOrderedMap::iterator=std::pair<Key,Value>\n")
wxT("wxOrderedMap::const_iterator=std::pair<Key,Value>\n");
}

TagsOptionsData::~TagsOptionsData()
{
}

void TagsOptionsData::Serialize(Archive &arch)
{
	// since of build 3749, we *always* set CC_ACCURATE_SCOPE_RESOLVING to true
	m_ccFlags |= CC_ACCURATE_SCOPE_RESOLVING;

	arch.Write     (wxT("m_ccFlags"),                m_ccFlags);
	arch.Write     (wxT("m_ccColourFlags"),          m_ccColourFlags);
	arch.WriteCData(wxT("m_tokens"),                 m_tokens);
	arch.WriteCData(wxT("m_types"),                  m_types);
	arch.Write     (wxT("m_fileSpec"),               m_fileSpec);
	arch.Write     (wxT("m_languages"),              m_languages);
	arch.Write     (wxT("m_minWordLen"),             m_minWordLen);
	arch.Write     (wxT("m_parserSearchPaths"),      m_parserSearchPaths);
	arch.Write     (wxT("m_parserEnabled"),          m_parserEnabled);
	arch.Write     (wxT("m_parserExcludePaths"),     m_parserExcludePaths);
	arch.Write     (wxT("m_maxItemToColour"),        m_maxItemToColour);
	arch.Write     (wxT("m_macrosFiles"),            m_macrosFiles);
	arch.Write     (wxT("m_clangOptions"),           m_clangOptions);
	arch.Write     (wxT("m_clangBinary"),            m_clangBinary);
	arch.WriteCData(wxT("m_clangCmpOptions"),        m_clangCmpOptions);
	arch.WriteCData(wxT("m_clangSearchPaths"),       m_clangSearchPaths);
	arch.WriteCData(wxT("m_clangMacros"),            m_clangMacros);
	arch.Write     (wxT("m_clangCachePolicy"),       m_clangCachePolicy);
	arch.Write     (wxT("m_ccNumberOfDisplayItems"), m_ccNumberOfDisplayItems);
}

void TagsOptionsData::DeSerialize(Archive &arch)
{
	arch.Read     (wxT("m_ccFlags"),                m_ccFlags);
	arch.Read     (wxT("m_ccColourFlags"),          m_ccColourFlags);
	arch.ReadCData(wxT("m_tokens"),                 m_tokens);
	arch.ReadCData(wxT("m_types"),                  m_types);
	arch.Read     (wxT("m_fileSpec"),               m_fileSpec);
	arch.Read     (wxT("m_languages"),              m_languages);
	arch.Read     (wxT("m_minWordLen"),             m_minWordLen);
	arch.Read     (wxT("m_parserSearchPaths"),      m_parserSearchPaths);
	arch.Read     (wxT("m_parserEnabled"),          m_parserEnabled);
	arch.Read     (wxT("m_parserExcludePaths"),     m_parserExcludePaths);
	arch.Read     (wxT("m_maxItemToColour"),        m_maxItemToColour);
	arch.Read     (wxT("m_macrosFiles"),            m_macrosFiles);
	arch.Read     (wxT("m_clangOptions"),           m_clangOptions);
	arch.Read     (wxT("m_clangBinary"),            m_clangBinary);
	arch.ReadCData(wxT("m_clangCmpOptions"),        m_clangCmpOptions);
	arch.ReadCData(wxT("m_clangSearchPaths"),       m_clangSearchPaths);
	arch.ReadCData(wxT("m_clangMacros"),            m_clangMacros);
	arch.Read     (wxT("m_clangCachePolicy"),       m_clangCachePolicy);
	arch.Read     (wxT("m_ccNumberOfDisplayItems"), m_ccNumberOfDisplayItems);
	// since of build 3749, we *always* set CC_ACCURATE_SCOPE_RESOLVING to true
	DoUpdateTokensWxMapReversed();
	DoUpdateTokensWxMap();

	m_ccFlags |= CC_ACCURATE_SCOPE_RESOLVING;
}

wxString TagsOptionsData::ToString()
{
	wxString options(wxEmptyString);

	static wxString file_name;
	wxString file_content;
	
	if(file_name.IsEmpty()) {
		char *ctagsReplacement = getenv("CTAGS_REPLACEMENTS");
		if(ctagsReplacement) {
			file_name = wxString(ctagsReplacement, wxConvUTF8).c_str();
		}
	}

	DoUpdateTokensWxMap();
	std::map<wxString, wxString> tokensMap      = GetTokensWxMap();
	std::map<wxString, wxString>::iterator iter = tokensMap.begin();

	if(tokensMap.empty() == false) {
		for(; iter != tokensMap.end(); iter++) {
			if(!iter->second.IsEmpty() || (iter->second.IsEmpty() && iter->first.Find(wxT("%0")) != wxNOT_FOUND)) {
				// Key = Value pair. Place this one in the output file
				file_content << iter->first << wxT("=") << iter->second << wxT("\n");
			} else {

				if(options.IsEmpty())
					options = wxT(" -I");

				options << iter->first;
				options << wxT(",");
			}
		}

		if(options.IsEmpty() == false)
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

const std::map<wxString, wxString>& TagsOptionsData::GetTokensWxMap() const
{
	return m_tokensWxMap;
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

		if(_IsValidCppIndetifier(k) && !_IsCppKeyword(k)) {
			tokens[k.mb_str(wxConvUTF8).data()] = v.mb_str(wxConvUTF8).data();
		}
	}
	return tokens;
}

void TagsOptionsData::SetTokens(const wxString& tokens)
{
	DoUpdateTokensWxMapReversed();
	DoUpdateTokensWxMap();

	this->m_tokens = tokens;
}

void TagsOptionsData::DoUpdateTokensWxMap()
{
	m_tokensWxMap.clear();
	wxArrayString tokensArr = wxStringTokenize(m_tokens, wxT("\r\n"), wxTOKEN_STRTOK);
	for (size_t i=0; i<tokensArr.GetCount(); i++) {
		wxString item = tokensArr.Item(i).Trim().Trim(false);
		wxString k = item.BeforeFirst(wxT('='));
		wxString v = item.AfterFirst(wxT('='));
		m_tokensWxMap[k] = v;
	}
}

void TagsOptionsData::DoUpdateTokensWxMapReversed()
{
	m_tokensWxMapReversed.clear();
	wxArrayString typesArr = wxStringTokenize(m_tokens, wxT("\r\n"), wxTOKEN_STRTOK);
	for (size_t i=0; i<typesArr.GetCount(); i++) {
		wxString item = typesArr.Item(i).Trim().Trim(false);
		wxString k = item.AfterFirst(wxT('='));
		wxString v = item.BeforeFirst(wxT('='));
		if(_IsValidCppIndetifier(k) && !_IsCppKeyword(k)) {
			m_tokensWxMapReversed[k] = v;
		}
	}
}
const std::map<wxString,wxString>& TagsOptionsData::GetTokensReversedWxMap() const
{
	return m_tokensWxMapReversed;
}

