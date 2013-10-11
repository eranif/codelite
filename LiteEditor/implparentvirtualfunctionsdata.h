//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : implparentvirtualfunctionsdata.h
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

#ifndef IMPLPARENTVIRTUALFUNCTIONSDATA_H
#define IMPLPARENTVIRTUALFUNCTIONSDATA_H

#include "serialized_object.h" // Base class

class ImplParentVirtualFunctionsData : public SerializedObject
{
	size_t m_flags;
public:
	enum {
		InsertDoxygenComment   = 0x00000001,
		FormatText             = 0x00000002,
		PrependVirtual         = 0x00000004,
		ImplParentVFuncDefault = InsertDoxygenComment | FormatText
	};

public:
	ImplParentVirtualFunctionsData()
			: m_flags(ImplParentVFuncDefault) {
	}

	virtual ~ImplParentVirtualFunctionsData() {
	}

public:
	virtual void DeSerialize(Archive &arch) {
		arch.Read(wxT("m_flags"), m_flags);
	}

	virtual void Serialize(Archive &arch) {
		arch.Write(wxT("m_flags"), m_flags);
	}

	void SetFlags(const size_t& flags) {
		this->m_flags = flags;
	}

	const size_t& GetFlags() const {
		return m_flags;
	}
};

#endif // IMPLPARENTVIRTUALFUNCTIONSDATA_H
