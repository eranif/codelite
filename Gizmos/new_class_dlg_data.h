//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : new_class_dlg_data.h
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

#ifndef NEWCLASSDLGDATA_H
#define NEWCLASSDLGDATA_H

#include "serialized_object.h" // Base class: SerializedObject

class NewClassDlgData : public SerializedObject
{
	size_t m_flags;
public:
	enum {
		Singleton               = 0x00000001,
		NonCopyable             = 0x00000002,
		VirtualDtor             = 0x00000004,
		ImplAllVirtualFuncs     = 0x00000008,
		ImplAllPureVirtualFuncs = 0x00000010,
		FileIniline             = 0x00000020,
		UseUnderscores          = 0x00000040,
		HppHeader               = 0x00000080
	};
	
public:
	NewClassDlgData();
	virtual ~NewClassDlgData();

public:
	virtual void DeSerialize(Archive& arch);
	virtual void Serialize(Archive& arch);

	void SetFlags(const size_t& flags) {
		this->m_flags = flags;
	}
	const size_t& GetFlags() const {
		return m_flags;
	}
};

#endif // NEWCLASSDLGDATA_H
