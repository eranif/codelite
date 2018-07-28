//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
    enum eOptions {
        Singleton = (1 << 0),
        NonCopyable = (1 << 1),
        VirtualDtor = (1 << 2),
        ImplAllVirtualFuncs = (1 << 3),
        ImplAllPureVirtualFuncs = (1 << 4),
        FileIniline = (1 << 5),
        UseUnderscores = (1 << 6),
        HppHeader = (1 << 7),
        UsePragma = (1 << 8),
        UseLowerCase = (1 << 9),
    };

public:
    NewClassDlgData();
    virtual ~NewClassDlgData();

public:
    virtual void DeSerialize(Archive& arch);
    virtual void Serialize(Archive& arch);

    void SetFlags(const size_t& flags) { this->m_flags = flags; }
    const size_t& GetFlags() const { return m_flags; }
    void EnableFlag(NewClassDlgData::eOptions flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }
    bool HasFlag(NewClassDlgData::eOptions flag) const { return m_flags & flag; }
};

#endif // NEWCLASSDLGDATA_H
