//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : WordCompletionDictionary.h
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

#ifndef WORDCOMPLETIONDICTIONARY_H
#define WORDCOMPLETIONDICTIONARY_H

#include "macros.h"
#include <wx/string.h>
#include <wx/event.h>
#include "WordCompletionThread.h"
#include "WordCompletionRequestReply.h"
#include "cl_command_event.h"

class WordCompletionDictionary : public wxEvtHandler
{
    std::map<wxString, wxStringSet_t> m_files;
    WordCompletionThread* m_thread;

protected:
    void OnEditorChanged(wxCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void OnFileSaved(clCommandEvent& event);

private:
    void DoCacheActiveEditor(bool overwrite);

public:
    WordCompletionDictionary();
    virtual ~WordCompletionDictionary();
    
    /**
     * @brief this function is called by the word completion thread when parsing phase is done
     * @param reply
     */
    void OnSuggestThread(const WordCompletionThreadReply& reply);
    
    /**
     * @brief return a set of words from the current editors
     */
    wxStringSet_t GetWords() const;
};

#endif // WORDCOMPLETIONDICTIONARY_H
