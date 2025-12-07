#include "XDebugCommandHandler.h"

XDebugCommandHandler::XDebugCommandHandler(XDebugManager* mgr, int transactionId)
    : m_mgr(mgr)
    , m_transactionId(transactionId)
{
}
