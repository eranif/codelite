//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gdbmi_parse_thread_info.cpp
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

#include "gdbmi_parse_thread_info.h"

void GdbMIThreadInfoParser::Parse(const wxString& info)
{
    m_threads.clear();
    // an example for -thread-info output
    // ^done,threads=[{id="30",target-id="Thread5060.0x1174",frame={level="0",addr="0x77a1000d",func="foo",args=[],from="C:\path\to\file"},state="stopped"},{..}],current-thread-id="30"
    wxString buffer = info;
    wxString threadsInfo;
    wxString threadBlock;
    
    if ( !ReadBlock(buffer, "[]", threadsInfo) )
        return;
    
    wxString activeThreadId;
    ReadKeyValuePair(buffer, "current-thread-id=", activeThreadId);
    
    while ( ReadBlock(threadsInfo, "{}", threadBlock) ) {
        GdbMIThreadInfo ti;
        ReadKeyValuePair(threadBlock, "id=",        ti.threadId);
        ReadKeyValuePair(threadBlock, "target-id=", ti.extendedName);
        ReadKeyValuePair(threadBlock, "func=",      ti.function);
        ReadKeyValuePair(threadBlock, "file=",      ti.file);
        ReadKeyValuePair(threadBlock, "line=",      ti.line);
        ti.active = activeThreadId == ti.threadId ? "Yes" : "No";
        m_threads.push_back( ti );
    }
}

bool GdbMIThreadInfoParser::ReadBlock(wxString& input, const wxString& pair, wxString& block)
{
    wxChar openChar  = pair.GetChar(0);
    wxChar closeChar = pair.GetChar(1);
    block.clear();
    int depth = 0;
    
    const int StateSearchStart = 0;
    const int StateCollecting  = 1;
    
    int curstate = StateSearchStart;
    for(size_t i=0; i<input.length(); ++i) {
        wxChar ch = input.GetChar(i);
        switch ( curstate ) {
        case StateSearchStart:
            if ( ch == openChar ) {
                depth++;
                curstate = StateCollecting;
            }
            break;
        case StateCollecting:
            // Incase the open/close are the same, always
            // test for closeure before an open
            if ( ch == closeChar ) {
                depth--;
                if ( depth == 0 ) {
                    // remove the content up to the current position
                    // from input and return the collected block
                    input = input.Mid(i);
                    return true;
                }
            } else if ( ch == openChar ) {
                depth++;
            }
            
            block << ch;
            break;
        }
    }
    return false;
}

bool GdbMIThreadInfoParser::ReadKeyValuePair(const wxString& input, const wxString& key, wxString& value)
{
    int where = input.Find(key);
    if ( where == wxNOT_FOUND )
        return false;
    
    wxString sub = input.Mid(where);
    return ReadBlock(sub, "\"\"", value);
}
