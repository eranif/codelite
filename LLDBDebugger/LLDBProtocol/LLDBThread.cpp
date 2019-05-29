//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBThread.cpp
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

#include "LLDBThread.h"
#include "LLDBEnums.h"

LLDBThread::LLDBThread()
    : m_id(wxNOT_FOUND)
    , m_line(wxNOT_FOUND)
    , m_active(false)
    , m_suspended(false)
    , m_stopReason( kStopReasonInvalid )
{
}

LLDBThread::~LLDBThread()
{
}

void LLDBThread::FromJSON(const JSONItem& json)
{
    m_id   = json.namedObject("m_id").toInt();
    m_func = json.namedObject("m_func").toString();
    m_file = json.namedObject("m_file").toString();
    m_line = json.namedObject("m_line").toInt();
    m_active = json.namedObject("m_active").toBool();
    m_suspended = json.namedObject("m_suspended").toBool();
    m_stopReason = json.namedObject("m_stopReason").toInt();
    m_stopReasonString = json.namedObject("m_stopReasonString").toString();
    m_name = json.namedObject("m_name").toString();
}

JSONItem LLDBThread::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("m_id", m_id);
    json.addProperty("m_func", m_func);
    json.addProperty("m_file", m_file);
    json.addProperty("m_line", m_line);
    json.addProperty("m_active", m_active);
    json.addProperty("m_suspended", m_suspended);
    json.addProperty("m_stopReason", m_stopReason);
    json.addProperty("m_stopReasonString", m_stopReasonString);
    json.addProperty("m_name", m_name);
    return json;
}

JSONItem LLDBThread::ToJSON(const LLDBThread::Vect_t& threads, const wxString &name)
{
    JSONItem arr = JSONItem::createArray(name);
    for(size_t i=0; i<threads.size(); ++i) {
        arr.arrayAppend( threads.at(i).ToJSON() );
    }
    return arr;
}

LLDBThread::Vect_t LLDBThread::FromJSON(const JSONItem& json, const wxString& name)
{
    LLDBThread::Vect_t v;
    JSONItem arr = json.namedObject(name);
    for(int i=0; i<arr.arraySize(); ++i) {
        LLDBThread thr;
        thr.FromJSON( arr.arrayItem(i) );
        v.push_back( thr );
    }
    return v;
}
