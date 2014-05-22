#ifndef CLCOMPILEREVENT_H
#define CLCOMPILEREVENT_H

#include <ICompilerLocator.h>
#include "cl_command_event.h" // Base class: clCommandEvent
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clCompilerEvent : public clCommandEvent
{
    ICompilerLocator::CompilerVec_t m_compilers;
public:
    clCompilerEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCompilerEvent(const clCompilerEvent& event);
    clCompilerEvent& operator=(const clCompilerEvent& src);
    virtual ~clCompilerEvent();
    virtual wxEvent *Clone() const {
        return new clCompilerEvent(*this);
    } 
    void SetCompilers(const ICompilerLocator::CompilerVec_t& compilers) {
        this->m_compilers = compilers;
    }
    const ICompilerLocator::CompilerVec_t& GetCompilers() const {
        return m_compilers;
    }
};

typedef void (wxEvtHandler::*clCompilerEventFunction)(clCompilerEvent&);
#define clCompilerEventHandler(func) \
    wxEVENT_HANDLER_CAST(clCompilerEventFunction, func)
    
#endif // CLCOMPILEREVENT_H
