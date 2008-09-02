#include "cscopestatusmessage.h"
CScopeStatusMessage::CScopeStatusMessage()
{
}

CScopeStatusMessage::~CScopeStatusMessage()
{
}

void CScopeStatusMessage::SetMessage(const wxString& message)
{
	this->m_message = message.c_str();
}

void CScopeStatusMessage::SetPercentage(const int& percentage)
{
	this->m_percentage = percentage;
}

const wxString& CScopeStatusMessage::GetMessage() const
{
	return m_message;
}

const int& CScopeStatusMessage::GetPercentage() const
{
	return m_percentage;
}
