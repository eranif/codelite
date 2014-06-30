//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : DiffConfig.h
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

#ifndef DIFFCONFIG_H
#define DIFFCONFIG_H

#include "cl_config.h" // Base class: clConfigItem
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK DiffConfig : public clConfigItem
{
public:
    enum {
        kCopyLeftToRightAndMove = 0x00000001,
        kCopyRightToLeftAndMove = 0x00000002,
    };
    
    // View mode
    enum {
        kViewModeDefault        = 0x00000000,
        kViewSingle             = 0x00000001,
        kViewVerticalSplit      = 0x00000002,
        kViewHorizontalSplit    = 0x00000004,
        kAllViewModes           = (kViewSingle|kViewVerticalSplit|kViewHorizontalSplit),
    };
    
protected:
    size_t m_flags;
    size_t m_viewFlags;
    
public:

    DiffConfig();
    virtual ~DiffConfig();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
    
    DiffConfig& SetViewMode(int mode) {
        m_viewFlags &= ~kAllViewModes;
        m_viewFlags |= mode;
        return *this;
    }
    
    bool IsSplitVertical() const {
        if (! (m_viewFlags & kAllViewModes) ) {
            // none of the modes is selected, select the default "Vertical"
            return true;
        }
        return m_viewFlags & kViewVerticalSplit;
    }
    
    bool IsSplitHorizontal() const {
        return m_viewFlags & kViewHorizontalSplit;
    }
    
    bool IsSingleViewMode() const {
        return m_viewFlags & kViewSingle;
    }
    
    void SetFlags(size_t flags) {
        this->m_flags = flags;
    }
    size_t GetFlags() const {
        return m_flags;
    }
    
    DiffConfig& Load();
    void Save();
};

#endif // DIFFCONFIG_H
