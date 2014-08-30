#ifndef XDEBUGCOMMANDHANDLER_H
#define XDEBUGCOMMANDHANDLER_H

#include <map>
#include <wx/sharedptr.h>

/// Base class for XDebug command handlers
class XDebugManager;
class wxXmlNode;
class wxSocketBase;
class XDebugCommandHandler
{
public:
    typedef wxSharedPtr<XDebugCommandHandler> Ptr_t;
    typedef std::map<int, XDebugCommandHandler::Ptr_t> Map_t;

protected:
    XDebugManager *m_mgr;
    int m_transactionId;

public:
    XDebugCommandHandler(XDebugManager* mgr, int transcationId);
    virtual ~XDebugCommandHandler();

    virtual void Process(const wxXmlNode* response) = 0;
    void SetTransactionId(int transactionId) {
        this->m_transactionId = transactionId;
    }
    int GetTransactionId() const {
        return m_transactionId;
    }
};

#endif // XDEBUGCOMMANDHANDLER_H
