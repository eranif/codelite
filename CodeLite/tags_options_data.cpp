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
#include <wx/ffile.h>
#include "tags_options_data.h"

//---------------------------------------------------------

TagsOptionsData::TagsOptionsData()
		: SerializedObject()
		, m_ccFlags(CC_DISP_FUNC_CALLTIP | CC_LOAD_EXT_DB | CC_CPP_KEYWORD_ASISST | CC_COLOUR_WORKSPACE_TAGS | CC_COLOUR_VARS)
		, m_ccColourFlags(CC_COLOUR_DEFAULT)
		, m_fileSpec(wxT("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++"))
		, m_minWordLen(3)
{
	m_languages.Add(wxT("C++"));
	m_languages.Add(wxT("Java"));
}

TagsOptionsData::~TagsOptionsData()
{
}

void TagsOptionsData::Serialize(Archive &arch)
{
	// Hack: remove "extern"
	int where = m_prep.Index(wxT("extern"));
	if (where != wxNOT_FOUND) {
		m_prep.RemoveAt(where);
	}

	arch.Write(wxT("m_ccFlags"), m_ccFlags);
	arch.Write(wxT("m_ccColourFlags"), m_ccColourFlags);
	arch.Write(wxT("m_prep"), m_prep);
	arch.Write(wxT("m_fileSpec"), m_fileSpec);
	arch.Write(wxT("m_languages"), m_languages);
	arch.Write(wxT("m_minWordLen"), m_minWordLen);
}

void TagsOptionsData::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_ccFlags"), m_ccFlags);
	if( !arch.Read(wxT("m_ccColourFlags"), m_ccColourFlags) ) {
		m_ccColourFlags = CC_COLOUR_DEFAULT;
	}
	
	arch.Read(wxT("m_prep"), m_prep);
	arch.Read(wxT("m_fileSpec"), m_fileSpec);
	arch.Read(wxT("m_languages"), m_languages);
	arch.Read(wxT("m_minWordLen"), m_minWordLen);

	// Hack: remove "extern"
	int where = m_prep.Index(wxT("extern"));
	if (where != wxNOT_FOUND) {
		m_prep.RemoveAt(where);
	}

	//TODO :: remove the CC_CACHE_WORKSPACE_TAGS
	m_ccFlags &= ~(CC_CACHE_WORKSPACE_TAGS);
}

wxString TagsOptionsData::ToString() const
{
	wxString options(wxEmptyString);

	wxString file_name, file_content;
	wxGetEnv(wxT("CTAGS_REPLACEMENTS"), &file_name);

	if (m_prep.GetCount() > 0) {
		options = wxT(" -I");
		for (size_t i=0; i<m_prep.GetCount(); i++) {
			wxString item = GetPreprocessor().Item(i);
			item = item.Trim().Trim(false);

			if (item.IsEmpty())
				continue;
			if (item.Find(wxT("=")) != wxNOT_FOUND) {
				// key=value item, this one is placed in the output file
				file_content << item << wxT("\n");
			} else {
				options += item;
				options += wxT(",");
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

std::map<std::string,std::string> TagsOptionsData::GetPreprocessorAsMap() const
{
	std::map<std::string,std::string> tokens;
	for (size_t i=0; i<m_prep.GetCount(); i++) {
		//const wxCharBuffer bufKey = _C(
		wxString item = m_prep.Item(i).Trim().Trim(false);
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

std::map<wxString, wxString> TagsOptionsData::GetPreprocessorAsWxMap() const
{
	std::map<wxString, wxString> tokens;
	for (size_t i=0; i<m_prep.GetCount(); i++) {
		wxString item = m_prep.Item(i).Trim().Trim(false);
		wxString k = item.BeforeFirst(wxT('='));
		wxString v = item.AfterFirst(wxT('='));
		tokens[k] = v;
	}
	return tokens;
}
