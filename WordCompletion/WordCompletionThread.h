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
    ~WordCompletionThread();
    virtual void ProcessRequest(ThreadRequest* request);
    
    /**
     * @brief parse 'buffer' and return set of words to complete
     */
    static void ParseBuffer(const wxString& buffer, wxStringSet_t& suggest);
};

#endif // WORDCOMPLETIONTHREAD_H
