#ifndef JSCODECOMPLETION_H
#define JSCODECOMPLETION_H

#include "ieditor.h"
#include <wx/event.h>
#include "smart_ptr.h"
#include "clTernServer.h"
#include "wxCodeCompletionBoxEntry.h"
#include "cl_calltip.h"

class JSCodeCompletion : public wxEvtHandler
{
    clTernServer m_ternServer;
    int m_ccPos;
    bool m_enabled;
public:
    typedef SmartPtr<JSCodeCompletion> Ptr_t;
    void OnCodeCompleteReady(const wxCodeCompletionBoxEntry::Vec_t& entries, const wxString& filename);
    void OnFunctionTipReady(clCallTipPtr calltip, const wxString& filename);
    
public:
    JSCodeCompletion();
    virtual ~JSCodeCompletion();
    
    /**
     * @brief code complete the current expression
     */
    void CodeComplete(IEditor *editor);
    
    /**
     * @brief restart the tern server
     */
    void Reload();
};

#endif // JSCODECOMPLETION_H
