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
