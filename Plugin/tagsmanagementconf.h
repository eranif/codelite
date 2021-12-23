//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tagsmanagementconf.h
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

#ifndef __tagsmanagementconf__
#define __tagsmanagementconf__

#include "serialized_object.h"

class TagsManagementConf : public SerializedObject
{
    wxArrayString m_files;

public:
    TagsManagementConf();
    virtual ~TagsManagementConf();

public:
    virtual void DeSerialize(Archive& arch);
    virtual void Serialize(Archive& arch);
    void SetFiles(const wxArrayString& files) { this->m_files = files; }
    const wxArrayString& GetFiles() const { return m_files; }
};
#endif // __tagsmanagementconf__
