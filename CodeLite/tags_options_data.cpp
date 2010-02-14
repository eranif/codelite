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
	m_languages.Add(wxT("C++"));
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
