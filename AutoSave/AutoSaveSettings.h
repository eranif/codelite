//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2016 The CodeLite Team
// File name            : AutoSaveSettings.h
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

#ifndef AUTOSAVESETTINGS_H
#define AUTOSAVESETTINGS_H

#include "cl_config.h"

class AutoSaveSettings : public clConfigItem
{
public:
    enum {
        kEnabled = (1 << 0),
    };

protected:
    size_t m_flags;
    size_t m_checkInterval;

public:
    AutoSaveSettings();
    ~AutoSaveSettings();

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    static AutoSaveSettings Load();
    static void Save(const AutoSaveSettings& settings);

    AutoSaveSettings& EnableFlag(int flag, bool b = true)
    {
        b ? (m_flags |= flag) : (m_flags &= ~flag);
        return *this;
    }

    bool HasFlag(int flag) const { return m_flags & flag; }
    AutoSaveSettings& SetFlags(size_t flags)
    {
        this->m_flags = flags;
        return *this;
    }
    size_t GetFlags() const { return m_flags; }
    AutoSaveSettings& SetCheckInterval(size_t checkInterval)
    {
        this->m_checkInterval = checkInterval;
        return *this;
    }
    size_t GetCheckInterval() const { return m_checkInterval; }
};

#endif // AUTOSAVESETTINGS_H
