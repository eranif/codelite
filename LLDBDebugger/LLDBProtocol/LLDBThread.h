//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBThread.h
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

#ifndef LLDBTHREAD_H
#define LLDBTHREAD_H

#include <wx/string.h>
#include "JSON.h"
#include <vector>

class LLDBThread
{
    int      m_id;
    wxString m_func;
    wxString m_file;
    int      m_line;
    bool     m_active;
    bool     m_suspended;
    int      m_stopReason;
    wxString m_stopReasonString;
    wxString m_name;

public:
    typedef std::vector<LLDBThread> Vect_t;

public:
    LLDBThread();
    virtual ~LLDBThread();


    void SetStopReasonString(const wxString& stopReasonString) {
        this->m_stopReasonString = stopReasonString;
    }
    const wxString& GetStopReasonString() const {
        return m_stopReasonString;
    }
    void SetStopReason(int stopReason) {
        this->m_stopReason = stopReason;
    }
    int GetStopReason() const {
        return m_stopReason;
    }
    void SetFile(const wxString& file) {
        this->m_file = file;
    }
    void SetFunc(const wxString& func) {
        this->m_func = func;
    }
    void SetId(int id) {
        this->m_id = id;
    }
    void SetLine(int line) {
        this->m_line = line;
    }
    const wxString& GetFile() const {
        return m_file;
    }
    const wxString& GetFunc() const {
        return m_func;
    }
    int GetId() const {
        return m_id;
    }
    int GetLine() const {
        return m_line;
    }

    void SetActive(bool active) {
        this->m_active = active;
    }
    bool IsActive() const {
        return m_active;
    }

    void SetSuspended(bool suspended) {
        this->m_suspended = suspended;
    }
    bool IsSuspended() const {
        return m_suspended;
    }

    void SetName(const char *name) {
        if(nullptr != name) {
            this->m_name = name;
        } else {
            m_name.Clear();
        }
    }
    const wxString &GetName() const {
        return m_name;
    }

    // Serialization API
    JSONItem ToJSON() const;
    void FromJSON(const JSONItem& json);

    static JSONItem ToJSON(const LLDBThread::Vect_t& threads, const wxString &name);
    static LLDBThread::Vect_t FromJSON(const JSONItem& json, const wxString &name);
};

#endif // LLDBTHREAD_H
