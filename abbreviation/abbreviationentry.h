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

#include "JSON.h"
#include "cl_config.h"
#include "serialized_object.h"

class AbbreviationEntry : public SerializedObject
{
    wxStringMap_t m_entries;
    bool m_autoInsert = false;

public:
    AbbreviationEntry() = default;
    ~AbbreviationEntry() override = default;

public:
    void DeSerialize(Archive& arch) override;
    void Serialize(Archive& arch) override;

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
    bool m_autoInsert = false;

public:
    AbbreviationJSONEntry()
        : clConfigItem("Abbreviations")
    {
    }

    ~AbbreviationJSONEntry() override = default;

    void FromJSON(const JSONItem& json) override;
    JSONItem ToJSON() const override;

    void SetEntries(const wxStringMap_t& entries) { this->m_entries = entries; }
    const wxStringMap_t& GetEntries() const { return m_entries; }
    void SetAutoInsert(bool autoInsert) { this->m_autoInsert = autoInsert; }
    bool IsAutoInsert() const { return m_autoInsert; }
};

#endif // __abbreviationentry__
