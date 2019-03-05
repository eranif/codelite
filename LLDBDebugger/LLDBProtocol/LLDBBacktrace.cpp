//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBBacktrace.cpp
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

#include "LLDBBacktrace.h"
#include <wx/filename.h>
#if BUILD_CODELITE_LLDB
LLDBBacktrace::LLDBBacktrace(lldb::SBThread& thread, const LLDBSettings& settings)
{
    m_callstack.clear();
    m_selectedFrameId = 0;
    if(thread.IsValid()) {

        m_threadId = thread.GetIndexID();
        m_selectedFrameId = thread.GetSelectedFrame().GetFrameID();

        int nFrames(0);
        // limit the number of frames to display
        thread.GetNumFrames() > settings.GetMaxCallstackFrames() ? nFrames = settings.GetMaxCallstackFrames() :
                                                                   nFrames = thread.GetNumFrames();
        for(int j = 0; j < nFrames; ++j) {

            lldb::SBFrame frame = thread.GetFrameAtIndex(j);
            LLDBBacktrace::Entry entry;

            if(frame.IsValid()) {
                // do we have a file:line?
                if(frame.GetLineEntry().IsValid()) {

                    lldb::SBFileSpec fileSepc = frame.GetLineEntry().GetFileSpec();
                    entry.filename = wxFileName(fileSepc.GetDirectory(), fileSepc.GetFilename()).GetFullPath();
                    entry.functionName = frame.GetFunctionName();
                    entry.line = frame.GetLineEntry().GetLine() - 1;
                    entry.id = j;
                    entry.address << wxString::Format("%p", (void*)frame.GetFP());
                    m_callstack.push_back(entry);

                } else {
                    // FIXME: if we dont have a debug symbol, we should learn how to construct a proper entry
                    // for now, we add an empty entry
                    entry.functionName = "??";
                    entry.id = j;
                    m_callstack.push_back(entry);
                }
            }
        }
    }
}
#endif

LLDBBacktrace::~LLDBBacktrace() {}

wxString LLDBBacktrace::ToString() const
{
    wxString str;
    str << "Thread ID: " << m_threadId << "\n";
    for(size_t i = 0; i < m_callstack.size(); ++i) {
        const LLDBBacktrace::Entry& entry = m_callstack.at(i);
        str << entry.address << ", " << entry.functionName << ", " << entry.filename << ", " << entry.line << "\n";
    }
    return str;
}

void LLDBBacktrace::FromJSON(const JSONItem& json)
{
    m_callstack.clear();
    m_threadId = json.namedObject("m_threadId").toInt(0);
    m_selectedFrameId = json.namedObject("m_selectedFrameId").toInt(0);
    JSONItem arr = json.namedObject("m_callstack");
    for(int i = 0; i < arr.arraySize(); ++i) {
        LLDBBacktrace::Entry entry;
        entry.FromJSON(arr.arrayItem(i));
        m_callstack.push_back(entry);
    }
}

JSONItem LLDBBacktrace::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("m_threadId", m_threadId);
    json.addProperty("m_selectedFrameId", m_selectedFrameId);

    JSONItem arr = JSONItem::createArray("m_callstack");
    json.append(arr);

    for(size_t i = 0; i < m_callstack.size(); ++i) {
        arr.append(m_callstack.at(i).ToJSON());
    }
    return json;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void LLDBBacktrace::Entry::FromJSON(const JSONItem& json)
{
    id = json.namedObject("id").toInt(0);
    line = json.namedObject("line").toInt(0);
    filename = json.namedObject("filename").toString();
    functionName = json.namedObject("functionName").toString();
    address = json.namedObject("address").toString();
}

JSONItem LLDBBacktrace::Entry::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("id", id);
    json.addProperty("line", line);
    json.addProperty("filename", filename);
    json.addProperty("functionName", functionName);
    json.addProperty("address", address);
    return json;
}
