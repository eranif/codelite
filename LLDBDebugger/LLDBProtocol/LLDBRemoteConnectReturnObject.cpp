#include "LLDBRemoteConnectReturnObject.h"

LLDBConnectReturnObject::LLDBConnectReturnObject()
    : m_pivotNeeded(false)
{
}

LLDBConnectReturnObject::~LLDBConnectReturnObject()
{
}

void LLDBConnectReturnObject::Clear()
{
    m_pivotNeeded = false;
    m_remoteHostName.Clear();
}
