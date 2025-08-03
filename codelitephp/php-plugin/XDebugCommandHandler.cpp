#include "XDebugCommandHandler.h"

XDebugCommandHandler::XDebugCommandHandler(XDebugManager* mgr, int transcationId)
    : m_mgr(mgr)
    , m_transactionId(transcationId)
{
}
