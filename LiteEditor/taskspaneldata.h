//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : taskspaneldata.h
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

#ifndef TASKSPANELDATA_H
#define TASKSPANELDATA_H

#include "serialized_object.h" // Base class

class TasksPanelData : public SerializedObject
{
    wxStringMap_t m_tasks;
    wxArrayString m_enabledItems;
    wxString m_encoding;

public:
    TasksPanelData()
    {
        // Set the default search patterns
        m_tasks.clear();
        m_tasks[wxT("TODO")] = wxT("/[/\\*]+ *TODO");
        m_tasks[wxT("BUG")] = wxT("/[/\\*]+ *BUG");
        m_tasks[wxT("ATTN")] = wxT("/[/\\*]+ *ATTN");
        m_tasks[wxT("FIXME")] = wxT("/[/\\*]+ *FIXME");

        m_enabledItems.Clear();
        m_enabledItems.Add(wxT("TODO"));
        m_enabledItems.Add(wxT("BUG"));
        m_enabledItems.Add(wxT("FIXME"));
        m_enabledItems.Add(wxT("ATTN"));
    }

    virtual ~TasksPanelData() {}

public:
    void SetEncoding(const wxString& encoding) { this->m_encoding = encoding; }
    const wxString& GetEncoding() const { return m_encoding; }
    void SetEnabledItems(const wxArrayString& enabledItems) { this->m_enabledItems = enabledItems; }
    void SetTasks(const wxStringMap_t& tasks) { this->m_tasks = tasks; }
    const wxArrayString& GetEnabledItems() const { return m_enabledItems; }
    const wxStringMap_t& GetTasks() const { return m_tasks; }

    virtual void DeSerialize(Archive& arch)
    {
        arch.Read(wxT("m_tasks"), m_tasks);
        arch.Read(wxT("m_enabledItems"), m_enabledItems);
        arch.Read(wxT("m_encoding"), m_encoding);
    }

    virtual void Serialize(Archive& arch)
    {
        arch.Write(wxT("m_tasks"), m_tasks);
        arch.Write(wxT("m_enabledItems"), m_enabledItems);
        arch.Write(wxT("m_encoding"), m_encoding);
    }
};

#endif // TASKSPANELDATA_H
