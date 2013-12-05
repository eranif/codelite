//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : generalinfo.h
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
#ifndef GENERALINFO_H
#define GENERALINFO_H

#include "wx/string.h"
#include "serialized_object.h"

enum {
    CL_MAXIMIZE_FRAME    = 0x00000001,
    CL_SHOW_WELCOME_PAGE = 0x00000004,
    CL_USE_EOL_LF        = 0x00000008,
    CL_USE_EOL_CR        = 0x00000010,
    CL_USE_EOL_CRLF      = 0x00000020,
    CL_SHOW_EOL          = 0x00000040,
    CL_SHOW_SPLASH       = 0x00000080
};

class GeneralInfo : public SerializedObject
{
    wxSize m_frameSize;
    wxPoint m_framePos;
    size_t m_flags;

public:
    GeneralInfo();
    virtual ~GeneralInfo();

    const wxSize& GetFrameSize() const {
        return m_frameSize;
    }
    void SetFrameSize(const wxSize &sz) {
        m_frameSize = sz;
    }

    const wxPoint& GetFramePosition() const {
        return m_framePos;
    }
    void SetFramePosition(const wxPoint &pt) {
        m_framePos = pt;
    }

    void SetFlags(const size_t& flags) {
        this->m_flags = flags;
    }
    const size_t& GetFlags() const {
        return m_flags;
    }

    void Serialize(Archive &arch);
    void DeSerialize(Archive &arch);
};

#endif //GENERALINFO_H
