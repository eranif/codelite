//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : abbreviationentry.h
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

#ifndef __abbreviationentry__
#define __abbreviationentry__

#include "cl_config.h"
#include "JSON.h"
#include "serialized_object.h"

class AbbreviationEntry : public SerializedObject
{
    wxStringMap_t m_entries;
    bool m_autoInsert;

public:
    AbbreviationEntry();
    virtual ~AbbreviationEntry();

public:
    virtual void DeSerialize(Archive& arch);
    virtual void Serialize(Archive& arch);

    // Setters
    void SetEntries(const wxStringMap_t& entries) { this->m_entries = entries; }
    void SetAutoInsert(const bool& autoInsert) { this->m_autoInsert = autoInsert; }

    // Getters
    const wxStringMap_t& GetEntries() const { return m_entries; }
    const bool& GetAutoInsert() const { return m_autoInsert; }
};

class AbbreviationJSONEntry : public clConfigItem
{
    wxStringMap_t m_entries;
    bool m_autoInsert;

public:
    AbbreviationJSONEntry()
        : clConfigItem("Abbreviations")
        , m_autoInsert(false)
    {
    }

    virtual ~AbbreviationJSONEntry() {}

    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    void SetEntries(const wxStringMap_t& entries) { this->m_entries = entries; }
    const wxStringMap_t& GetEntries() const { return m_entries; }
    void SetAutoInsert(bool autoInsert) { this->m_autoInsert = autoInsert; }
    bool IsAutoInsert() const { return m_autoInsert; }
};

#endif // __abbreviationentry__
