//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : WordCompletionThread.h
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

#ifndef WORDCOMPLETIONTHREAD_H
#define WORDCOMPLETIONTHREAD_H

#include "worker_thread.h"
#include <wx/string.h>
#include <wx/filename.h>
#include "macros.h"
#include "WordCompletionRequestReply.h"

class WordCompletionDictionary;
class WordCompletionThread : public WorkerThread
{
protected:
    WordCompletionDictionary* m_dict;
    
public:

    WordCompletionThread(WordCompletionDictionary* dict);
    ~WordCompletionThread() = default;
    virtual void ProcessRequest(ThreadRequest* request);
    
    /**
     * @brief parse 'buffer' and return set of words to complete
     */
    static void ParseBuffer(const wxString& buffer, wxStringSet_t& suggest);
};

#endif // WORDCOMPLETIONTHREAD_H
