#ifndef JSCODECOMPLETION_H
#define JSCODECOMPLETION_H

#include "ieditor.h"
#include <wx/event.h>
#include "smart_ptr.h"
#include "JSParserThread.h"
#include "clTernServer.h"
#include "wxCodeCompletionBoxEntry.h"

class JSCodeCompletion : public wxEvtHandler
{
    JSParserThread* m_thread;
    clTernServer m_ternServer;
    int m_ccPos;
    
public:
    typedef SmartPtr<JSCodeCompletion> Ptr_t;
    void OnCodeCompleteReady(const wxCodeCompletionBoxEntry::Vec_t& entries, const wxString& filename);
    
public:
    JSCodeCompletion();
    virtual ~JSCodeCompletion();
    
    /**
     * @brief code complete the current expression
     */
    void CodeComplete(IEditor *editor);
    
    void PraserThreadCompleted(JSParserThread::Reply* reply);
};

#endif // JSCODECOMPLETION_H
