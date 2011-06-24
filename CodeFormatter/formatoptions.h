//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : formatoptions.h
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
#ifndef FORMATOPTIONS_H
#define FORMATOPTIONS_H

#include "serialized_object.h"

enum AstyleOptions {
	AS_ANSI						= 0x00000001,
	AS_GNU						= 0x00000002,
	AS_KR						= 0x00000004,
	AS_LINUX					= 0x00000008,
	AS_JAVA						= 0x00000010,
	AS_BRACKETS_BREAK			= 0x00000020,
	AS_BRACKETS_ATTACH			= 0x00000040,
	AS_BRACKETS_LINUX			= 0x00000080,
	AS_BRACKETS_BREAK_CLOSING	= 0x00000100,
	AS_INDENT_CLASS				= 0x00000200,
	AS_INDENT_SWITCHES			= 0x00000400,
	AS_INDENT_CASE				= 0x00000800,
	AS_INDENT_BLOCKS			= 0x00001000,
	AS_INDENT_BRACKETS			= 0x00002000,
	AS_INDENT_NAMESPACES		= 0x00004000,
	AS_INDENT_LABELS			= 0x00008000,
	AS_INDENT_PREPROCESSORS		= 0x00010000,
	AS_MAX_INSTATEMENT_INDENT	= 0x00020000,
	AS_BREAK_BLOCKS				= 0x00040000,
	AS_BREAK_BLOCKS_ALL			= 0x00080000,
	AS_BREAK_ELSEIF				= 0x00100000,
	AS_PAD_OPER					= 0x00200000,
	AS_PAD_PARENTHESIS			= 0x00400000,
	AS_PAD_PARENTHESIS_OUT		= 0x00800000,
	AS_PAD_PARENTHESIS_IN		= 0x01000000,
	AS_UNPAD_PARENTHESIS		= 0x02000000,
	AS_ONE_LINE_KEEP_STATEMENT	= 0x04000000,
	AS_ONE_LINE_KEEP_BLOCKS		= 0x08000000,
	AS_FILL_EMPTY_LINES			= 0x10000000,
	AS_MIN_COND_INDENT			= 0x20000000,
	AS_INDENT_USES_TABS			= 0x40000000,
	AS_DEFAULT					= AS_LINUX | AS_BRACKETS_LINUX | AS_INDENT_PREPROCESSORS
};

class FormatOptions : public SerializedObject
{
	size_t m_options;
	wxString m_customFlags;

public:
	FormatOptions();
	virtual ~FormatOptions();

	wxString ToString() const;

	void Serialize(Archive &arch);
	void DeSerialize(Archive &arch);

	size_t GetOptions() const {
		return m_options;
	}
	void SetOption(size_t options) {
		m_options = options;
	}
	void SetCustomFlags(const wxString& customFlags) {
		this->m_customFlags = customFlags;
	}
	const wxString& GetCustomFlags() const {
		return m_customFlags;
	}
};

#endif //FORMATOPTIONS_H
