#include "LLDBRemoteConnectReturnObject.h"

LLDBRemoteConnectReturnObject::LLDBRemoteConnectReturnObject()
    : m_pivotNeeded(false)
{
}

LLDBRemoteConnectReturnObject::~LLDBRemoteConnectReturnObject()
{
}

void LLDBRemoteConnectReturnObject::Clear()
{
    m_pivotNeeded = false;
    m_remoteHostName.Clear();
}
