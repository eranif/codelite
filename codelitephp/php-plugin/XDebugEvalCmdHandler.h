#ifndef XDEBUGEVALCMDHANDLER_H
#define XDEBUGEVALCMDHANDLER_H

#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler
#include <wx/string.h>

class XDebugManager;
class XDebugEvalCmdHandler : public XDebugCommandHandler
{
public:
    enum {
        kEvalForTooltip  = 1,
        kEvalForEvalPane = 2,
    };
protected:
    wxString m_expression;
    int      m_evalReason;
    
public:
    XDebugEvalCmdHandler(const wxString &expression, int evalReason, XDebugManager* mgr, int transcationId);
    virtual ~XDebugEvalCmdHandler();

    void SetExpression(const wxString& expression) {
        this->m_expression = expression;
    }
    const wxString& GetExpression() const {
        return m_expression;
    }
public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGEVALCMDHANDLER_H
