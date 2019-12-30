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

#include "serialized_object.h"
#include "wx/string.h"

enum {
    CL_MAXIMIZE_FRAME = (1 << 0),
    CL_SHOW_WELCOME_PAGE = (1 << 2),
    CL_USE_EOL_LF = (1 << 3),
    CL_USE_EOL_CR = (1 << 4),
    CL_USE_EOL_CRLF = (1 << 5),
    CL_SHOW_EOL = (1 << 6),
    CL_SHOW_SPLASH = (1 << 7),
    CL_FULLSCREEN = (1 << 8),
};

class GeneralInfo : public SerializedObject
{
    wxSize m_frameSize;
    wxPoint m_framePos;
    size_t m_flags = CL_MAXIMIZE_FRAME | CL_USE_EOL_LF;

public:
    GeneralInfo();
    virtual ~GeneralInfo();

    const wxSize& GetFrameSize() const { return m_frameSize; }
    void SetFrameSize(const wxSize& sz) { m_frameSize = sz; }

    const wxPoint& GetFramePosition() const { return m_framePos; }
    void SetFramePosition(const wxPoint& pt) { m_framePos = pt; }

    void SetFlags(const size_t& flags) { this->m_flags = flags; }
    const size_t& GetFlags() const { return m_flags; }

    void Serialize(Archive& arch);
    void DeSerialize(Archive& arch);
};

#endif // GENERALINFO_H
