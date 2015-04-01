#ifndef JSCODECOMPLETION_H
#define JSCODECOMPLETION_H

#include "ieditor.h"
#include <wx/event.h>
#include "smart_ptr.h"
#include "JSParserThread.h"

class JSCodeCompletion : public wxEvtHandler
{
    JSParserThread* m_thread;
    
public:
    typedef SmartPtr<JSCodeCompletion> Ptr_t;
    
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
