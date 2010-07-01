#include "commitmessagescache.h"

void CommitMessagesCache::GetMessages(wxArrayString& messages, wxArrayString& previews)
{
	for(size_t i=0; i<m_messages.GetCount(); i++) {
		messages.Add(m_messages.Item(i));
		previews.Add(m_messages.Item(i).BeforeFirst(wxT('\n')));
	}
}

void CommitMessagesCache::AddMessage(const wxString& message)
{
	wxString formattedMessage = FormatMessage(message);
	if(formattedMessage.IsEmpty())
		return;

	if(m_messages.Index(formattedMessage) == wxNOT_FOUND) {
		m_messages.Add(formattedMessage);
	}
}

wxString CommitMessagesCache::FormatMessage(const wxString& message)
{
	wxString formattedMessage ( message );
	formattedMessage.Replace(wxT("\r\n"), wxT("\n"));
	formattedMessage.Replace(wxT("\v"),   wxT("\n"));
	formattedMessage.Trim(false).Trim();
	return formattedMessage;
}
