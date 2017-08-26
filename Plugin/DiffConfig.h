//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
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
        kCopyLeftToRightAndMove = (1 << 0),
        kCopyRightToLeftAndMove = (1 << 1),
        diffShowLineNumbers = (1 << 2),
        diffHideOverviewBar = (1 << 3),
    };

    // View mode
    enum {
        kViewModeDefault = 0,
        kViewSingle = (1 << 0),
        kViewVerticalSplit = (1 << 1),
        kViewHorizontalSplit = (1 << 2),
        kAllViewModes = (kViewSingle | kViewVerticalSplit | kViewHorizontalSplit),
    };

protected:
    size_t m_flags;
    size_t m_viewFlags;
    wxString m_leftFile;
    wxString m_rightFile;

public:
    DiffConfig();
    virtual ~DiffConfig();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    DiffConfig& SetViewMode(int mode)
    {
        m_viewFlags &= ~kAllViewModes;
        m_viewFlags |= mode;
        return *this;
    }

    bool IsSplitVertical() const
    {
        if(!(m_viewFlags & kAllViewModes)) {
            // none of the modes is selected, select the default "Vertical"
            return true;
        }
        return m_viewFlags & kViewVerticalSplit;
    }

    bool IsSplitHorizontal() const { return m_viewFlags & kViewHorizontalSplit; }

    bool IsSingleViewMode() const { return m_viewFlags & kViewSingle; }

    bool ShowLineNumbers() const { return m_flags & diffShowLineNumbers; }
    void SetShowLineNumbers(bool flag) {
        m_flags &= ~diffShowLineNumbers;
        if (flag) {
            m_flags |= diffShowLineNumbers;
        }
    }

    bool HideOverviewBar() const { return m_flags & diffHideOverviewBar; }
    void SetHideOverviewBar(bool flag) {
        m_flags &= ~diffHideOverviewBar;
        if (flag) {
            m_flags |= diffHideOverviewBar;
        }
    }

    void SetFlags(size_t flags) { this->m_flags = flags; }
    size_t GetFlags() const { return m_flags; }

    DiffConfig& SetLeftFile(const wxString& leftFile)
    {
        this->m_leftFile = leftFile;
        return *this;
    }
    DiffConfig& SetRightFile(const wxString& rightFile)
    {
        this->m_rightFile = rightFile;
        return *this;
    }
    const wxString& GetLeftFile() const { return m_leftFile; }
    const wxString& GetRightFile() const { return m_rightFile; }
    DiffConfig& Load();
    void Save();
};

#endif // DIFFCONFIG_H
