//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_options_data.h
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
#ifndef TAGS_OPTIONS_DATA_H
#define TAGS_OPTIONS_DATA_H

#include "serialized_object.h"
#include "wx/filename.h"
#include <map>

enum CodeCompletionOpts {
	CC_PARSE_COMMENTS			= 0x00000001,
	CC_DISP_COMMENTS			= 0x00000002,
	CC_DISP_TYPE_INFO			= 0x00000004,
	CC_DISP_FUNC_CALLTIP		= 0x00000008,
	CC_LOAD_EXT_DB				= 0x00000010,
	CC_LOAD_EXT_DB_TO_MEMORY	= 0x00000020,
	CC_PARSE_EXT_LESS_FILES		= 0x00000040,
	CC_COLOUR_VARS				= 0x00000080,
	CC_COLOUR_WORKSPACE_TAGS	= 0x00000100,
	CC_CPP_KEYWORD_ASISST		= 0x00000200,
	CC_CACHE_WORKSPACE_TAGS		= 0x00000400,
	CC_DISABLE_AUTO_PARSING		= 0x00000800,
	CC_MARK_TAGS_FILES_IN_BOLD	= 0x00001000
};

enum CodeCompletionColourOpts {
	CC_COLOUR_CLASS				= 0x00000001,
	CC_COLOUR_STRUCT			= 0x00000002,
	CC_COLOUR_FUNCTION			= 0x00000004,
	CC_COLOUR_ENUM				= 0x00000008,
	CC_COLOUR_UNION				= 0x00000010,
	CC_COLOUR_PROTOTYPE			= 0x00000020,
	CC_COLOUR_TYPEDEF			= 0x00000040,
	CC_COLOUR_MACRO				= 0x00000080,
	CC_COLOUR_NAMESPACE			= 0x00000100,
	CC_COLOUR_DEFAULT			= 	CC_COLOUR_CLASS | CC_COLOUR_STRUCT | CC_COLOUR_FUNCTION |
									CC_COLOUR_ENUM | CC_COLOUR_PROTOTYPE
};

class TagsOptionsData : public SerializedObject
{
	size_t m_ccFlags;
	size_t m_ccColourFlags;

	wxArrayString m_prep;
	wxString m_fileSpec;
	wxArrayString m_languages;
	int m_minWordLen;

public:
	TagsOptionsData();
	virtual ~TagsOptionsData();

	//setters/getters
	void SetFlags(const size_t &flags) {
		m_ccFlags = flags;
	}
	void SetFileSpec(const wxString &filespec) {
		m_fileSpec = filespec;
	}
	void SetPreprocessor(const wxArrayString& prep) {
		m_prep = prep;
	}
	void SetLanguages(const wxArrayString &langs) {
		m_languages = langs;
	}
	void SetLanguageSelection(const wxString &lang);

	const wxArrayString &GetLanguages() const {
		return m_languages;
	}
	const wxArrayString& GetPreprocessor() const {
		return m_prep;
	}
	std::map<std::string, std::string> GetPreprocessorAsMap() const;
	std::map<wxString, wxString> GetPreprocessorAsWxMap() const;

	const size_t& GetFlags() const {
		return m_ccFlags;
	}
	const wxString &GetFileSpec() const {
		return m_fileSpec;
	}

	//Setters
	void SetMinWordLen(const int& minWordLen) {
		this->m_minWordLen = minWordLen;
	}
	//Getters
	const int& GetMinWordLen() const {
		return m_minWordLen;
	}

	//Serialization API
	void Serialize(Archive &arch);
	void DeSerialize(Archive &arch);

	wxString ToString() const;

	void SetCcColourFlags(const size_t& ccColourFlags) {
		this->m_ccColourFlags = ccColourFlags;
	}
	
	const size_t& GetCcColourFlags() const {
		return m_ccColourFlags;
	}
};

#endif //TAGS_OPTIONS_DATA_H
