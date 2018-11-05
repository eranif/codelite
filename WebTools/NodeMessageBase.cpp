#include "NodeMessageBase.h"

NodeMessageBase::NodeMessageBase(const wxString& eventName)
    : m_eventName(eventName)
{
}

NodeMessageBase::~NodeMessageBase() {}
