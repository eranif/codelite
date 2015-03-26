#ifndef JSCODECOMPLETION_H
#define JSCODECOMPLETION_H

#include "ieditor.h"
#include <wx/event.h>
#include "JSLookUpTable.h"
#include "smart_ptr.h"
#include "JSParserThread.h"

class JSCodeCompletion : public wxEvtHandler
{
    JSLookUpTable::Ptr_t m_lookup;
    JSParserThread* m_thread;
    
public:
    typedef SmartPtr<JSCodeCompletion> Ptr_t;
    
protected:
    int GetImgIndex(JSObject::Ptr_t obj);
    JSObject::Map_t GetObjectProperties(JSObject::Ptr_t o);
    
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
